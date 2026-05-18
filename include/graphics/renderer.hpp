#pragma once
#include "common/common_graphics.hpp"
#include <memory>

namespace slabb::graphics
{
	class RenderGraph;
}

namespace slabb::graphics::wrapper
{
	class Device;
	class Instance;
	class Swapchain;
}

namespace slabb::graphics::wrapper::command
{
	class CommandAllocator;
	class CommandQueue;
}

namespace slabb::graphics::wrapper::descriptor
{
	class DescriptorHeap;
}

using Microsoft::WRL::ComPtr;
using slabb::graphics::wrapper::Device;
using slabb::graphics::wrapper::Instance;
using slabb::graphics::wrapper::Swapchain;
using slabb::graphics::wrapper::command::CommandAllocator;
using slabb::graphics::wrapper::command::CommandQueue;
using slabb::graphics::wrapper::descriptor::DescriptorHeap;

namespace slabb::graphics
{
	class SLABB_EXPORT Renderer {
	public:
		Renderer(UINT window_width, UINT window_height);
		~Renderer();
		bool init_backend(HWND hWnd);
		void render_frame();
	private:
		std::unique_ptr<RenderGraph> m_render_graph;
		std::unique_ptr<Instance> m_instance;
		std::unique_ptr<Device> m_device;
		std::unique_ptr<Swapchain> m_swapchain;
		std::unique_ptr<CommandAllocator> m_cmd_allocator;
		std::unique_ptr<CommandQueue> m_cmd_queue;
		std::unique_ptr<DescriptorHeap> m_descriptor_heap;
	};
}

