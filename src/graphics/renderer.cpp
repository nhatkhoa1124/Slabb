#include "graphics/renderer.hpp"
#include <directx/d3dx12.h>
#include <spdlog/spdlog.h>
#include <DirectXMath.h>

#include "graphics/render_graph.hpp"
#include "graphics/wrapper/instance.hpp"
#include "graphics/wrapper/device.hpp"
#include "graphics/wrapper/swapchain.hpp"
#include "graphics/wrapper/command/command_allocator.hpp"
#include "graphics/wrapper/command/command_list.hpp"
#include "graphics/wrapper/command/command_queue.hpp"
#include "graphics/wrapper/resource/descriptor_heap.hpp"
#include "graphics/wrapper/pipeline/graphics_pipeline.hpp"
#include "graphics/wrapper/root_signature.hpp"
#include "graphics/wrapper/synchronization/fence.hpp"
#include "graphics/graphics_interface/graphics_vertex.hpp"
#include "graphics/graphics_interface/graphics_shader.hpp"

using namespace slabb::graphics;
using namespace slabb::graphics::wrapper;
using namespace slabb::graphics::wrapper::descriptor;

namespace slabb::graphics
{
	Renderer::Renderer(UINT window_width, UINT window_height)
	{
		m_render_graph = std::make_unique<RenderGraph>();
		m_instance = std::make_unique<Instance>();
		m_device = std::make_unique<Device>();
		m_swapchain = std::make_unique<Swapchain>(window_width, window_height, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_cmd_queue = std::make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_cmd_list = std::make_unique<CommandList>();
		m_cmd_allocators.resize(m_swapchain->buffer_count());
		for (auto& allocator : m_cmd_allocators)
		{
			allocator = std::make_unique<CommandAllocator>();
		}
		m_fences.resize(m_swapchain->buffer_count());
		for (auto& fence : m_fences)
		{
			fence = std::make_unique<Fence>();
		}
		m_descriptor_heap = std::make_unique<DescriptorHeap>();
		m_graphics_pipeline = std::make_unique<GraphicsPipeline>();
		m_global_root_signature = std::make_unique<RootSignature>();
	}

	Renderer::~Renderer()
	{
		if (m_cmd_queue && !m_fences.empty())
		{
			m_fences[0]->flush(m_cmd_queue->command_queue());
		}
	}

	bool Renderer::init_backend(HWND hWnd)
	{
		// Factory and adapter creation
		m_instance->create_factory();
		m_instance->enumerate_adapter();
		// Device creation
		m_device->create_device(m_instance->adapter());
		// Command queue creation
		m_cmd_queue->create_command_queue(m_device->device());
		// Swapchain creation
		m_swapchain->create_swapchain(hWnd, m_cmd_queue->command_queue(), m_instance->factory());
		m_swapchain->get_buffers();
		// Command allocator creation
		for (auto& allocator : m_cmd_allocators)
		{
			allocator->create_allocator(m_device->device(), m_cmd_queue->command_list_type());
		}
		// Fence creation
		for (auto& fence : m_fences)
		{
			fence->create_fence(m_device->device());
		}
		// Descriptor heaps creation
		m_descriptor_heap->create_heap(DescriptorHeapType::RENDER_TARGET, m_device->device(), 2);
		m_descriptor_heap->create_heap(DescriptorHeapType::DEPTH, m_device->device(), 3);
		m_descriptor_heap->create_heap(DescriptorHeapType::RESOURCE, m_device->device(), 1024);
		// Render target view declaration
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(m_descriptor_heap->get_rtv_heap_start());
		// Backbuffer creation
		m_graph_backbuffers.resize(m_swapchain->buffer_count());
		for (UINT i = 0; i < m_swapchain->buffer_count(); i++)
		{
			ID3D12Resource* backbuffer = m_swapchain->render_target(i);
			m_device->device()->CreateRenderTargetView(backbuffer, nullptr, rtv_handle);
			rtv_handle.Offset(1, m_descriptor_heap->rtv_heap_size());

			m_graph_backbuffers[i] = m_render_graph->create_resource<TextureResource>(
				"BackBuffer_" + std::to_string(i),
				TextureUsage::BACK_BUFFER);
			m_graph_backbuffers[i]->set_native_resource(backbuffer);
			// Constant buffer creation
			std::string cb_name = "CameraCB_" + std::to_string(i);
			m_render_graph->create_resource<BufferResource>(cb_name, BufferUsage::CONSTANT);
			if (auto* cb = m_render_graph->get_resource<BufferResource>(cb_name))
			{
				TransformCB transform_data{};
				transform_data.mvp_matrix = DirectX::XMMatrixIdentity();
				cb->stage_data(&transform_data, sizeof(TransformCB));
				cb->initialize_hardware(m_device->device(), 0);
			}
		}



		return true;
	}

	bool Renderer::init_default_pipeline(const std::string& vertex_path, const std::string& pixel_path,
								  std::vector <core::VertexAttribute> vertex_attributes)
	{
		assert(!vertex_path.empty());
		assert(!pixel_path.empty());
		// Create root signature & graphics pipeline
		m_global_root_signature->serialize_root_signature();
		m_global_root_signature->create_root_signature(m_device->device());
		m_graphics_pipeline->load_shaders(vertex_path, pixel_path);
		const auto& input_elements = GraphicsVertex::get_vertex_input_layout(vertex_attributes);
		m_graphics_pipeline->create_graphics_pipeline(m_device->device(), m_global_root_signature->root_signature(),
													  input_elements);
		// Create command list
		// Hard-coded to use the first allocator here!!!
		m_cmd_list->create_command_list(m_device->device(), m_cmd_queue->command_list_type(),
										m_cmd_allocators[0]->allocator(), nullptr);
		m_cmd_list->close();
		// Compile render graph
		auto& main_pass = m_render_graph->add_pass("Main");

		main_pass.set_root_signature(m_global_root_signature->root_signature());
		main_pass.set_pipeline_state(m_graphics_pipeline->pipeline_state_object());

		auto* camera_cb = m_render_graph->get_resource<BufferResource>("CameraCB");
		if (camera_cb)
		{
			main_pass.reads_from(camera_cb);
		}

		main_pass.record([this](CommandList& cmd)
			{
				UINT current_frame = m_swapchain->current_backbuffer();

				CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(
					m_descriptor_heap->get_rtv_heap_start(),
					current_frame,
					m_descriptor_heap->rtv_heap_size()
				);
				cmd.set_render_target(1, &rtv_handle, nullptr);
				const float clear_color[] = { 0.05f, 0.05f, 0.05f, 1.0f };
				cmd.command_list()->ClearRenderTargetView(rtv_handle, clear_color, 0, nullptr);

				std::string cb_name = "CameraCB_" + std::to_string(current_frame);
				auto* active_cb = m_render_graph->get_resource<BufferResource>(cb_name);
				if (active_cb)
				{
					cmd.set_graphics_root_cbv(0, active_cb->constant_view().BufferLocation);
				}

				for (const auto& model : m_scene_models)
				{
					for (const auto& mesh : model.sub_meshes)
					{
						cmd.set_vertex_buffers(0, 1, &mesh.vertex_buffer->vertex_view());

						if (mesh.index_buffer)
						{
							cmd.command_list()->IASetIndexBuffer(&mesh.index_buffer->index_view());
							cmd.command_list()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
							cmd.command_list()->DrawIndexedInstanced(mesh.index_count, 1, 0, 0, 0);
						}
						else
						{
							cmd.command_list()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
							cmd.draw_instanced(mesh.vertex_count, 1, 0, 0);
						}
					}
				}
			});
		m_render_graph->compile();

		return true;
	}

	void Renderer::load_model(const GraphicsModel& model)
	{
		spdlog::info("Loading model...");
		RenderModel render_model;
		render_model.transform = model.transform;
		const auto& main_pass = m_render_graph->get_pass("Main");
		for (const auto& mesh : model.meshes)
		{
			RenderMesh gpu_mesh;
			gpu_mesh.vertex_count = static_cast<uint32_t>(mesh.vertex_count);
			gpu_mesh.index_count = static_cast<uint32_t>(mesh.index_count);

			gpu_mesh.vertex_buffer = m_render_graph->create_resource<BufferResource>("VertexBuffer", BufferUsage::VERTEX);
			gpu_mesh.vertex_buffer->stage_data(mesh.vertex_data, mesh.vertex_count * mesh.vertex_stride);
			gpu_mesh.vertex_buffer->initialize_hardware(m_device->device(), static_cast<UINT>(mesh.vertex_stride));

			if (mesh.index_count != 0)
			{
				gpu_mesh.index_buffer = m_render_graph->create_resource<BufferResource>("IndexBuffer", BufferUsage::INDEX);
				gpu_mesh.index_buffer->stage_data(mesh.index_data, mesh.index_count * sizeof(uint32_t));
				gpu_mesh.index_buffer->initialize_hardware(m_device->device(), sizeof(uint32_t), DXGI_FORMAT_R32_UINT);
			}
			// Setup main pass read resources
			if (gpu_mesh.vertex_buffer) main_pass->reads_from(gpu_mesh.vertex_buffer);
			if (gpu_mesh.index_buffer)  main_pass->reads_from(gpu_mesh.index_buffer);
			render_model.sub_meshes.push_back(gpu_mesh);
		}

		m_scene_models.push_back(render_model);
		spdlog::info("Model loaded successfully");
	}

	void Renderer::render_frame()
	{
		UINT current_frame = m_swapchain->current_backbuffer();
		m_fences[current_frame]->flush(m_cmd_queue->command_queue());

		// Hard-coding rotation logic here!!!
		static float rotation_angle = 0.0f;
		rotation_angle += 0.02f;

		DirectX::XMMATRIX model = DirectX::XMMatrixRotationZ(rotation_angle);

		// 2. For modern 3D movement, you would multiply this by your View and Projection matrices:
		// DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(...);
		// DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(...);
		// DirectX::XMMATRIX mvp  = model * view * proj;

		// 3. Transpose the matrix so HLSL reads it column-major properly
		TransformCB updated_payload;
		updated_payload.mvp_matrix = DirectX::XMMatrixTranspose(model);

		// 4. Safely overwrite the buffer designated exclusively for this frame index
		std::string cb_name = "CameraCB_" + std::to_string(current_frame);
		if (auto* active_cb = m_render_graph->get_resource<BufferResource>(cb_name))
		{
			active_cb->stage_data(&updated_payload, sizeof(TransformCB));

			// Because upload memory maps instantly, update the pointer mapping block:
			active_cb->hardware_heap()->upload_data(active_cb->raw_data());
		}

		TextureResource* current_backbuffer = m_graph_backbuffers[current_frame];
		const auto& main_pass = m_render_graph->get_pass("Main");

		main_pass->clear_write_targets();
		main_pass->writes_to(current_backbuffer);

		float width = static_cast<float>(m_swapchain->width());
		float height = static_cast<float>(m_swapchain->height());

		D3D12_VIEWPORT viewport = { 0.0f, 0.0f, width, height, 0.0f, 1.0f };
		D3D12_RECT scissor_rect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };

		main_pass->set_viewport(viewport);
		main_pass->set_rect(scissor_rect);

		// Dynamically compute layout transitions 
		m_render_graph->build_resource_barriers();

		// Record commands
		auto& allocator = m_cmd_allocators[current_frame];
		allocator->reset();
		m_cmd_list->reset(allocator->allocator(), nullptr);

		m_render_graph->render(*m_cmd_list);

		CD3DX12_RESOURCE_BARRIER present_barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			current_backbuffer->underlying_resource(),
			current_backbuffer->current_state(),
			D3D12_RESOURCE_STATE_PRESENT
		);
		m_cmd_list->set_resource_barrier(1, &present_barrier);
		m_cmd_list->close();

		ID3D12CommandList* pp_cmd_list[] = { m_cmd_list->command_list() };
		m_cmd_queue->execute_command_list(1, pp_cmd_list);
		m_swapchain->present(1, 0);
	}

}