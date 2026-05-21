#include "graphics/wrapper/command/command_list.hpp"

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::wrapper::command
{
	CommandList::CommandList()
	{
	}

	void CommandList::create_command_list(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type,
										  ID3D12CommandAllocator* allocator, ID3D12PipelineState* pipeline_state)
	{
		NULL_CHECK(device);
		NULL_CHECK(allocator);
		spdlog::info("Creating command list...");
		SLABB_CHECK(device->CreateCommandList(0, type, allocator, pipeline_state, IID_PPV_ARGS(&m_cmd_list)));
		spdlog::info("Command list created successfully");
	}

	void CommandList::close()
	{
		SLABB_CHECK(m_cmd_list->Close());
	}
}