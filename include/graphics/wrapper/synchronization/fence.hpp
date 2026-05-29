#pragma once 
#include "common/common_graphics.hpp"
#include <directx/d3d12.h>

using Microsoft::WRL::ComPtr;

namespace slabb::graphics::wrapper::synchronization
{
	class SLABB_EXPORT Fence
	{
	public:
		Fence();
		void create_fence(ID3D12Device* device);
		void create_fence_event();
	private:
		ComPtr<ID3D12Fence> m_fence;
		HANDLE m_fence_event;
	};
}