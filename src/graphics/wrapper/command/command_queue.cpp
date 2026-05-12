#include "graphics/wrapper/command/command_queue.hpp"
#include <d3d12.h>

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::wrapper::command
{
	CommandQueue::CommandQueue()
	{

	}

	void CommandQueue::create_command_queue(ID3D12Device* device)
	{
		if (!device)
		{
			spdlog::error("ERROR: No device detected while creating Command Queue");
			throw std::runtime_error("NULLPTR ERROR");
		}
	}
}