#include "graphics/wrapper/command/command_queue.hpp"

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::wrapper::command
{
	CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE cmd_list_type):
		m_list_type {cmd_list_type}
	{}

	void CommandQueue::create_command_queue(ID3D12Device* device)
	{
		NULL_CHECK(device);
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc =
		{
			.Type = m_list_type,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0
		};
		spdlog::trace("Creating Command Queue");
		SLABB_CHECK(device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmd_queue)));
	}
}