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
		void wait_for_fence(ID3D12CommandQueue* cmd_queue);

		[[nodiscard]] inline ID3D12Fence* fence() const { return m_fence.Get(); }
		[[nodiscard]] inline UINT fence_value() const { return m_fence_value; }
		[[nodiscard]] inline void* fence_event() const { return m_fence_event; }
	private:
		ComPtr<ID3D12Fence> m_fence;
		UINT m_fence_value;
		HANDLE m_fence_event;
	};
}