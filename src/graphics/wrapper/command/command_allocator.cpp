#include "graphics/wrapper/command/command_allocator.hpp"

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::wrapper::command
{
	CommandAllocator::CommandAllocator()
	{

	}

	void CommandAllocator::create_allocator(ID3D12Device* device, const D3D12_COMMAND_LIST_TYPE cmd_list_type)
	{
		NULL_CHECK(device);
		spdlog::info("Creating command allocator...");
		SLABB_CHECK(device->CreateCommandAllocator(cmd_list_type, IID_PPV_ARGS(&m_cmd_allocator)));
		spdlog::info("Command allocator created successfully");
	}
}