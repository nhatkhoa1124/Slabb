#include "graphics/renderer.hpp"

#include "graphics/render_graph.hpp"
#include "graphics/wrapper/instance.hpp"
#include "graphics/wrapper/device.hpp"
#include "graphics/wrapper/swapchain.hpp"
#include "graphics/wrapper/command/command_allocator.hpp"
#include "graphics/wrapper/command/command_queue.hpp"
#include "graphics/wrapper/descriptor/descriptor_heap.hpp"

using namespace slabb::graphics::wrapper::descriptor;

namespace slabb::graphics
{
	Renderer::Renderer(UINT window_width, UINT window_height)
	{
		m_render_graph = std::make_unique<RenderGraph>();
		m_instance = std::make_unique<Instance>();
		m_device = std::make_unique<Device>();
		m_cmd_queue = std::make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_cmd_allocator = std::make_unique<CommandAllocator>();
		m_swapchain = std::make_unique<Swapchain>(window_width, window_height, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_descriptor_heap = std::make_unique<DescriptorHeap>();
	}

	Renderer::~Renderer()
	{

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
		// Command allocator creation
		m_cmd_allocator->create_allocator(m_device->device(), m_cmd_queue->command_list_type());
		// Swapchain creation
		m_swapchain->create_swapchain(hWnd, m_cmd_queue->command_queue(), m_instance->factory());
		m_swapchain->get_buffers();
		// Descriptor heaps creation
		m_descriptor_heap->create_heap(HeapType::RENDER_TARGET, m_device->device(), 2);
		m_descriptor_heap->create_heap(HeapType::DEPTH, m_device->device(), 3);
		m_descriptor_heap->create_heap(HeapType::RESOURCE, m_device->device(), 1024);

		return true;
	}

	void Renderer::render_frame()
	{

	}

}