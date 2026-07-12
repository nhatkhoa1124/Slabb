#pragma once
#include <memory>

#include "graphics/wrapper/command/command_allocator.hpp"
#include "graphics/wrapper/synchronization/fence.hpp"
#include "render_graph.hpp"

namespace slabb::graphics
{
	/**
	* @brief Holds context and data necessary to render in a frame
	*/
	struct FrameContext
	{
		std::unique_ptr<wrapper::command::CommandAllocator> command_allocator;
		std::unique_ptr<wrapper::synchronization::Fence> fence;

		BufferResource* camera_constant_buffer{ nullptr };

		TextureResource* backbuffer_target{ nullptr };
		TextureResource* depth_target{ nullptr };
	};
}
