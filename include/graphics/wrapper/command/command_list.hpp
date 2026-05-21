#pragma once
#include "common/common_graphics.hpp"
#include <directx/d3d12.h>

using Microsoft::WRL::ComPtr;

namespace slabb::graphics::wrapper::command
{
	/**
	* @brief Wrapper class for command list operations
	*/
	class SLABB_EXPORT CommandList
	{
	public:
		CommandList();
		void create_command_list(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type, 
								 ID3D12CommandAllocator* allocator, ID3D12PipelineState* pipeline_state = nullptr);
		void close();
		inline void set_pipline_state(ID3D12PipelineState* pipeline_state) const
		{
			m_cmd_list->SetPipelineState(pipeline_state); 
		}

		[[nodiscard]] inline ID3D12GraphicsCommandList* command_list() const { return m_cmd_list.Get(); }
	private:
		ComPtr<ID3D12GraphicsCommandList> m_cmd_list;
	};
}