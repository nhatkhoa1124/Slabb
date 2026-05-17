#include "graphics/wrapper/swapchain.hpp"

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::wrapper
{
	Swapchain::Swapchain(const int width, const int height, DXGI_FORMAT format) : m_format {format}
	{
		assert(width > 0 && height > 0);
		m_width = width;
		m_height = height;
	}

	void Swapchain::get_buffers()
	{
		assert(!m_render_targets.empty());
		for (size_t i = 0; i < m_render_targets.size(); i++)
		{
			SLABB_CHECK(m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_render_targets[i])));
		}
	}

	void Swapchain::create_swapchain(HWND hWnd, ID3D12CommandQueue* cmd_queue, IDXGIFactory4* factory)
	{
		NULL_CHECK(hWnd);
		NULL_CHECK(cmd_queue);
		NULL_CHECK(factory);
		ComPtr<IDXGISwapChain1> swapchain1;
		
		DXGI_SWAP_CHAIN_DESC1 desc = {
			.Width = m_width,
			.Height = m_height,
			.Format = m_format,
			.SampleDesc = 
			{
				.Count = 1,
				.Quality = 0
			},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = m_buffer_counts,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD
		};
		spdlog::info("Creating swapchain...");

		SLABB_CHECK(factory->CreateSwapChainForHwnd(
			cmd_queue, hWnd, &desc, nullptr, nullptr, swapchain1.GetAddressOf()));
		SLABB_CHECK(swapchain1.As(&m_swapchain));
		spdlog::info("Swaphain created successfully");
	}

	void Swapchain::set_width(const int width)
	{
		assert(width > 0);
		m_width = static_cast<UINT>(width);
	}

	void Swapchain::set_height(const int height)
	{
		assert(height > 0);
		m_height = static_cast<UINT>(height);
	}
}