#include "graphics/wrapper/command/command_queue.hpp"
#include <directx/d3d12.h>

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::wrapper::command
{
	CommandQueue::CommandQueue()
	{

	}

	void CommandQueue::create_command_queue(ID3D12Device* device)
	{
		NULL_CHECK(device);
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc =
		{
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0
		};
		spdlog::trace("Creating Command Queue");
		SLABB_CHECK(device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmd_queue)));
	}
}