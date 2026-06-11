#pragma once
#include "common/common_graphics.hpp"
#include <directx/d3d12.h>

using Microsoft::WRL::ComPtr;

namespace slabb::graphics::wrapper::command
{
	class SLABB_EXPORT CommandQueue
	{
	public:
		CommandQueue(D3D12_COMMAND_LIST_TYPE cmd_list_type);
		void create_command_queue(ID3D12Device* device);
		
		/**
		* @brief Executes a list of command lists
		* @param count The number of command lists to execute
		* @param pp_cmd_list Pointer to the command lists array
		*/
		void execute_command_list(UINT count, ID3D12CommandList* const* pp_cmd_list);

		[[nodiscard]] ID3D12CommandQueue* command_queue() const { return m_cmd_queue.Get(); }
		[[nodiscard]] D3D12_COMMAND_LIST_TYPE command_list_type() const { return m_list_type; }
	private:
		ComPtr<ID3D12CommandQueue> m_cmd_queue;
		D3D12_COMMAND_LIST_TYPE m_list_type;
	};
}