#pragma once
#include "common/common_graphics.hpp"
#include <dxgi1_6.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

namespace slabb::graphics::wrapper
{
	class SLABB_EXPORT Swapchain
	{
	public:
		Swapchain(const int width, const int height, DXGI_FORMAT format);
		void create_swapchain(HWND hWnd, ID3D12CommandQueue* cmd_queue, IDXGIFactory2* factory);

		[[nodiscard]] inline IDXGISwapChain4* swapchain() const { return m_swapchain.Get(); }
		[[nodiscard]] inline UINT width() const { return m_width; }
		[[nodiscard]] inline UINT height() const { return m_height; }
		[[nodiscard]] inline DXGI_FORMAT format() const { return m_format; }
		[[nodiscard]] inline UINT current_backbuffer() { return m_swapchain->GetCurrentBackBufferIndex(); }
		void set_width(const int width);
		void set_height(const int height);
		inline void set_format(DXGI_FORMAT format) { m_format = format; }
	private:
		ComPtr<IDXGISwapChain4> m_swapchain;

		UINT m_width;
		UINT m_height;
		DXGI_FORMAT m_format;
		UINT m_buffer_counts = 2; // Front + Back buffers
	};
}