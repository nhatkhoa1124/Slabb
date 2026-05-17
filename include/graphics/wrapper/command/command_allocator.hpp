#pragma once
#include "common/common_graphics.hpp"
#include <directx/d3d12.h>

using Microsoft::WRL::ComPtr;

namespace slabb::graphics::wrapper::command
{
	class SLABB_EXPORT CommandAllocator
	{
	public:
		CommandAllocator();

		/**
		* @brief Creates a command allocator
		* @param device The pointer to the device that owns the allocator
		* @param cmd_list_type The command list type, must match with the command queue's list type
		*/
		void create_allocator(ID3D12Device* device, const D3D12_COMMAND_LIST_TYPE cmd_list_type);

		[[nodiscard]] inline ID3D12CommandAllocator* allocator() const { return m_cmd_allocator.Get(); }

	private:
		ComPtr<ID3D12CommandAllocator> m_cmd_allocator;
	};
}