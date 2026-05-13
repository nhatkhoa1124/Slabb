#include "graphics/wrapper/swapchain.hpp"

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::wrapper
{
	Swapchain::Swapchain(int width, int height, DXGI_FORMAT format) :
		m_width {width}, m_height {height}, m_format {format}
	{}

	void Swapchain::create_swapchain(HWND hWnd, ID3D12CommandQueue* cmd_queue, IDXGIFactory2* factory)
	{
		NULL_CHECK(hWnd);
		NULL_CHECK(cmd_queue);
		NULL_CHECK(factory);
		ComPtr<IDXGISwapChain1> swapchain1;
		
		DXGI_SWAP_CHAIN_DESC1 desc = {
			.Width = m_width,
			.Height = m_height,
			.Format = m_format,
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = 2, // Front + Back buffers
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD
		};
		spdlog::trace("Creating swapchain");

		SLABB_CHECK(factory->CreateSwapChainForHwnd(
			cmd_queue, hWnd, &desc, nullptr, nullptr, swapchain1.ReleaseAndGetAddressOf()));

	}
}