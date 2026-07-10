#include "graphics/renderer.hpp"
#include <directx/d3dx12.h>

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
		// Render target view & graph backbuffers creation
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(m_descriptor_heap->get_rtv_heap_start());
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
		}
		return true;
	}

	bool Renderer::init_default_pipeline(const std::string& vertex_path, const std::string& pixel_path,
								  std::vector <core::VertexAttribute> vertex_attributes)
	{
		assert(!vertex_path.empty());
		assert(!pixel_path.empty());
		// Create root signature & graphics pipeline
		std::unique_ptr<RootSignature> default_signature = std::make_unique<RootSignature>();
		default_signature->serialize_root_signature();
		default_signature->create_root_signature(m_device->device());
		m_graphics_pipeline->load_shaders(vertex_path, pixel_path);
		const auto& input_elements = GraphicsVertex::get_vertex_input_layout(vertex_attributes);
		m_graphics_pipeline->create_graphics_pipeline(m_device->device(), default_signature->root_signature(),
													  input_elements);
		// Create command list
		// Hard-coded to use the first allocator here!!!
		m_cmd_list->create_command_list(m_device->device(), m_cmd_queue->command_list_type(),
										m_cmd_allocators[0]->allocator(), nullptr);
		m_cmd_list->close();

		return true;
	}

	void Renderer::render_frame()
	{
		// Pre-draw setups
		UINT current_frame = m_swapchain->current_backbuffer();
		m_fences[current_frame]->flush(m_cmd_queue->command_queue());
		m_render_graph->clear();

		float width = static_cast<float>(m_swapchain->width());
		float height = static_cast<float>(m_swapchain->height());
		D3D12_VIEWPORT viewport = {};
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = width;
		viewport.Height = height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		D3D12_RECT scissor_rect = {};
		scissor_rect.left = 0;
		scissor_rect.top = 0;
		scissor_rect.right = static_cast<LONG>(width);
		scissor_rect.bottom = static_cast<LONG>(height);

		TextureResource* current_backbuffer = m_graph_backbuffers[current_frame]; // Get current backbuffer resource
		auto& main_pass = m_render_graph->add_pass("Main");
		main_pass.writes_to(current_backbuffer);
		main_pass.set_viewport(viewport);
		main_pass.set_rect(scissor_rect);

		// Record draw logic
		main_pass.record([this, current_frame](CommandList cmd)
			{
				CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(
					m_descriptor_heap->get_rtv_heap_start(),
					current_frame,
					m_descriptor_heap->rtv_heap_size()
				);
				cmd.set_render_target(1, &rtv_handle, nullptr);
				const float clear_color[] = { 0.1f, 0.2f, 0.3f, 1.0f };
				cmd.clear_render_target(rtv_handle, clear_color);
				cmd.set_pipline_state(m_graphics_pipeline->pipeline_state_object());
				cmd.draw_instanced(3, 1, 0, 0);
			});

		m_render_graph->compile();

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