#include "graphics/renderer.hpp"

#include "graphics/render_graph.hpp"
#include "graphics/wrapper/instance.hpp"
#include "graphics/wrapper/device.hpp"
#include "graphics/wrapper/swapchain.hpp"
#include "graphics/wrapper/command/command_allocator.hpp"
#include "graphics/wrapper/command/command_list.hpp"
#include "graphics/wrapper/command/command_queue.hpp"
#include "graphics/wrapper/descriptor/descriptor_heap.hpp"
#include "graphics/wrapper/pipeline/graphics_pipeline.hpp"
#include "graphics/wrapper/root_signature.hpp"
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
		m_cmd_queue = std::make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_cmd_list = std::make_unique<CommandList>();
		m_cmd_allocators.resize(m_swapchain->buffer_count());
		for (auto& allocator : m_cmd_allocators)
		{
			allocator = std::make_unique<CommandAllocator>();
		}
		m_swapchain = std::make_unique<Swapchain>(window_width, window_height, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_descriptor_heap = std::make_unique<DescriptorHeap>();
		m_graphics_pipeline = std::make_unique<GraphicsPipeline>();
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
		// Swapchain creation
		m_swapchain->create_swapchain(hWnd, m_cmd_queue->command_queue(), m_instance->factory());
		m_swapchain->get_buffers();
		// Command allocator creation
		for (auto& allocator : m_cmd_allocators)
		{
			allocator->create_allocator(m_device->device(), m_cmd_queue->command_list_type());
		}
		// Descriptor heaps creation
		m_descriptor_heap->create_heap(HeapType::RENDER_TARGET, m_device->device(), 2);
		m_descriptor_heap->create_heap(HeapType::DEPTH, m_device->device(), 3);
		m_descriptor_heap->create_heap(HeapType::RESOURCE, m_device->device(), 1024);

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

	}

}