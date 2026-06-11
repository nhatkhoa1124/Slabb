#include "graphics/render_graph.hpp"
#include <unordered_map>
#include <algorithm>

#include "graphics/tools/debug.hpp"

namespace slabb::graphics
{
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

	void RenderPass::record(std::function<void(wrapper::command::CommandList&)> callback)
	{
		m_callback = callback;
	}

	// Render Graph
	RenderGraph::RenderGraph(int width, int height) : m_render_pass{"demo"}
	{
		D3D12_VIEWPORT viewport = {};
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		m_render_pass.viewport = viewport;

		D3D12_RECT scissor_rect = {};
		scissor_rect.left = 0;
		scissor_rect.top = 0;
		scissor_rect.right = static_cast<LONG>(width);
		scissor_rect.bottom = static_cast<LONG>(height);
		m_render_pass.rect = scissor_rect;
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

	RenderPass& RenderGraph::add_pass(std::string name)
	{
		auto pass = std::make_unique<RenderPass>(std::move(name));
		RenderPass& ref = *pass;
		m_render_passes.push_back(std::move(pass));
		return ref;
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
		holder.resize(m_render_passes.size());

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
		spdlog::debug("Render graph compiled successfully");
	}
}