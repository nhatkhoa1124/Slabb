#include "graphics/render_graph.hpp"
#include <unordered_map>
#include <algorithm>
#include <directx/d3dx12.h>

#include "graphics/tools/debug.hpp"

namespace slabb::graphics
{
	// Buffer Resource
	void BufferResource::initialize_hardware(ID3D12Device* device, UINT stride_in_bytes, DXGI_FORMAT index_format)
	{
		if (m_data.empty()) return;
		UINT buffer_size = m_data.size();

		// Calculate the 256-byte aligned size required by the driver
		UINT raw_size = static_cast<UINT>(m_data.size());
		UINT aligned_size = (raw_size + 255) & ~255;

		// TODO: Using UPLOAD type for testing now, refactor for other uses
		m_hardware_heap = std::make_unique<wrapper::resource::BufferHeap>(wrapper::resource::HeapType::UPLOAD);
		m_hardware_heap->create_heap(device, buffer_size);

		m_hardware_heap->upload_data(std::span<const std::byte>(m_data));

		ID3D12Resource* native_res = m_hardware_heap->resource_heap();

		if (m_usage == BufferUsage::VERTEX)
		{
			m_vertex_view.BufferLocation = native_res->GetGPUVirtualAddress();
			m_vertex_view.StrideInBytes = stride_in_bytes;
			m_vertex_view.SizeInBytes = buffer_size;
		}
		else if (m_usage == BufferUsage::INDEX)
		{
			m_index_view.BufferLocation = native_res->GetGPUVirtualAddress();
			m_index_view.SizeInBytes = buffer_size;
			m_index_view.Format = index_format;
		}
		else if (m_usage == BufferUsage::CONSTANT)
		{
			m_constant_view.BufferLocation = native_res->GetGPUVirtualAddress();
			m_constant_view.SizeInBytes = aligned_size;
		}
	}

	//Render Pass
	void RenderPass::writes_to(const RenderResource* resource)
	{
		NULL_CHECK(resource);
		m_writes.push_back(resource);
	}

	void RenderPass::reads_from(const RenderResource* resource)
	{
		NULL_CHECK(resource);
		m_reads.push_back(resource);
	}

	void RenderPass::clear_write_targets()
	{
		m_writes.clear();
	}

	void RenderPass::record(std::function<void(wrapper::command::CommandList&)> callback)
	{
		m_callback = callback;
	}

	void RenderPass::execute(wrapper::command::CommandList& cmd_list)
	{
		if (!m_barriers.empty())
		{
			cmd_list.command_list()->ResourceBarrier(
				static_cast<UINT>(m_barriers.size()),
				m_barriers.data());
		}

		if (m_viewport.Width > 0.0f && m_viewport.Height > 0.0f)
		{
			cmd_list.set_viewport(1, &m_viewport);
		}
		if (m_rect.right > 0 && m_rect.bottom > 0)
		{
			cmd_list.set_scissor_rect(1, &m_rect);
		}
		if (m_root_signature)
		{
			cmd_list.command_list()->SetGraphicsRootSignature(m_root_signature);
		}
		if (m_pso)
		{
			cmd_list.set_pipline_state(m_pso);
		}

		if (m_callback)
		{
			m_callback(cmd_list);
		}
	}

	// Render Graph
	RenderGraph::RenderGraph()
	{
	}

	size_t RenderGraph::find_producer(const RenderResource* resource)
	{
		for (size_t i = 0; i < m_render_passes.size(); i++)
		{
			auto& pass = m_render_passes[i];
			const auto& writes = pass->write_resources();
			const auto& it = std::find(writes.begin(), 
								writes.end(), 
								resource);
			if (it != pass->write_resources().end())
			{
				return i;
			}
		}

		return SLABB_INVALID_SIZE_T;
	}

	std::vector<size_t> RenderGraph::find_consumers(const RenderResource* resource)
	{
		std::vector<size_t> indices{};

		for (size_t i = 0; i < m_render_passes.size(); i++)
		{
			auto& pass = m_render_passes[i];
			const auto& reads = pass->read_resources();
			const auto& it = std::find(reads.begin(),
								reads.end(), 
								resource);
			if (it != pass->read_resources().end())
			{
				indices.push_back(i);
			}
		}
		return indices;
	}

	void RenderGraph::build_resource_barriers()
	{
		spdlog::debug("Start building resource barriers...");

		for (auto& resource : m_resources)
		{
			if (auto* buffer = dynamic_cast<BufferResource*>(resource.get()))
			{
				if (buffer->hardware_heap_type() == D3D12_HEAP_TYPE_UPLOAD)
				{
					resource->set_state(D3D12_RESOURCE_STATE_GENERIC_READ);
					continue;
				}
			}
			resource->set_state(D3D12_RESOURCE_STATE_COMMON);
		}

		for (size_t pass_idx : m_execution_queue)
		{
			auto& pass = m_render_passes[pass_idx];
			pass->clear_barriers();

			auto process_resource = [&](const RenderResource* res, bool is_writer)
				{
					// Get physical resource
					ID3D12Resource* native_res = res->underlying_resource();
					if (!native_res) return;

					D3D12_RESOURCE_STATES current = res->current_state();
					D3D12_RESOURCE_STATES required = evalute_state(res, is_writer);

					// If current state is not the required state
					if (current != required)
					{
						pass->add_resource_barrier(CD3DX12_RESOURCE_BARRIER::Transition(
							native_res,
							current,
							required));
						// Remember the updated state
						const_cast<RenderResource*>(res)->set_state(required);
					}
				};
			for (const auto* write : pass->write_resources()) { process_resource(write, true); }
			for (const auto* read : pass->read_resources()) { process_resource(read, false); }
		}
		spdlog::debug("Resource barriers built succesfully");
	}

	D3D12_RESOURCE_STATES RenderGraph::evalute_state(const RenderResource* resource, bool is_writer)
	{
		if (const auto* buffer = dynamic_cast<const BufferResource*>(resource))
		{
			// 1. If it's an UPLOAD heap buffer, keep the permanent restriction rule:
			if (buffer->hardware_heap_type() == D3D12_HEAP_TYPE_UPLOAD)
			{
				return D3D12_RESOURCE_STATE_GENERIC_READ;
			}

			// 2. If it's a DEFAULT heap buffer, return the exact, precise state it needs!
			if (buffer->usage() == BufferUsage::VERTEX)
			{
				return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			}
			else if (buffer->usage() == BufferUsage::INDEX)
			{
				return D3D12_RESOURCE_STATE_INDEX_BUFFER;
			}
		}

		const auto* texture = dynamic_cast<const TextureResource*>(resource);
		if(is_writer)
		{
			if (texture && texture->usage() == TextureUsage::DEPTH_STENCIL_BUFFER)
			{
				return D3D12_RESOURCE_STATE_DEPTH_WRITE;
			}
			return texture ? D3D12_RESOURCE_STATE_RENDER_TARGET : D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		}
		else
		{
			if (texture && texture->usage() == TextureUsage::DEPTH_STENCIL_BUFFER)
			{
				return D3D12_RESOURCE_STATE_DEPTH_READ;
			}
			return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		}

	}

	RenderPass& RenderGraph::add_pass(std::string name)
	{
		auto pass = std::make_unique<RenderPass>(std::move(name));
		RenderPass& ref = *pass;
		m_render_passes.push_back(std::move(pass));
		return ref;
	}

	RenderPass* const RenderGraph::get_pass(std::string name) const
	{
		for (const auto& pass : m_render_passes)
		{
			if (name == pass->name())
			{
				return pass.get();
			}
		}
	}

	// Enum for node visiting
	enum class NodeState
	{
		UNVISITED,
		VISITING,
		VISITED		// Nodes that are fully visited and have no more children
	};

	void RenderGraph::compile()
	{
		// Construct adjacency list
		std::unordered_map<size_t, std::vector<size_t>> graph{};
		spdlog::debug("Constructing DAG graph...");
		for (const auto& resource : m_resources)
		{
			auto idx_producer = find_producer(resource.get());
			auto consumer_list = find_consumers(resource.get());

			if (idx_producer == SLABB_INVALID_SIZE_T)
			{
				spdlog::debug("Resource {} is an imported root input (No producer pass).", resource->name());
				continue;
			}
			if (consumer_list.empty())
			{
				spdlog::debug("Resource {} is a terminal frame output (No consumer passes).", resource->name());
			}

			for (const auto& idx_consumer : consumer_list)
			{
				graph[idx_producer].push_back(idx_consumer);
			}
		}
		spdlog::debug("DAG graph constructed successfully");

		// Graph traversing logic
		m_execution_queue.clear();
		// Track node visit state
		std::vector<NodeState> visit_states(m_render_passes.size(), NodeState::UNVISITED);
		// For holding the reversed order of the true execution queue, since we traverse from bottom up
		std::vector<size_t> holder;
		holder.reserve(m_render_passes.size());

		auto dfs = [&](auto& self, size_t pass_idx)
			{
				if(visit_states[pass_idx] == NodeState::VISITING)
				{
					spdlog::critical("Detected cycle inside render graph");
					throw std::runtime_error("Render graph contains a cyclic dependency loop");
				}
				if (visit_states[pass_idx] == NodeState::VISITED)
				{
					return;
				}

				visit_states[pass_idx] = NodeState::VISITING;
				const auto& it = graph.find(pass_idx);
				if (it != graph.end())
				{
					const std::vector<size_t>& consumers = it->second;

					for (size_t consumer_idx : consumers)
					{
						self(self, consumer_idx);
					}
				}

				visit_states[pass_idx] = NodeState::VISITED;
				holder.push_back(pass_idx);
			};

		for (size_t i = 0; i < m_render_passes.size(); i++)
		{
			if (visit_states[i] == NodeState::UNVISITED)
			{
				dfs(dfs, i);
			}
		}
		
		m_execution_queue = std::move(holder);
		std::reverse(m_execution_queue.begin(), m_execution_queue.end());
		build_resource_barriers();
		spdlog::debug("Render graph compiled successfully");
	}

	void RenderGraph::render(wrapper::command::CommandList& cmd_list)
	{
		for (size_t pass_idx : m_execution_queue)
		{
			m_render_passes[pass_idx]->execute(cmd_list);
		}
	}

	void RenderGraph::clear()
	{
		m_execution_queue.clear();
		m_render_passes.clear();
	}
}