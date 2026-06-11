#pragma once
#include "common/common_graphics.hpp"
#include <dxgi1_6.h>
#include <directx/d3d12.h>
#include <array>

using Microsoft::WRL::ComPtr;

namespace slabb::graphics::wrapper
{
	class SLABB_EXPORT Swapchain
	{
	public:
		Swapchain(const int width, const int height, DXGI_FORMAT format);

		/**
		* @brief Create a D3D12 swapchain object 
		* @param hWnd The handle to a window
		* @param cmd_queue The command queue pointer
		* @param factory The instance's factory pointer 
		*/
		void create_swapchain(HWND hWnd, ID3D12CommandQueue* cmd_queue, IDXGIFactory4* factory);

		/**
		* @brief Get and populate buffers for render targets
		*/
		void get_buffers();
		
		void present(UINT sync_interval, UINT flags);

		[[nodiscard]] IDXGISwapChain4* swapchain() const { return m_swapchain.Get(); }
		[[nodiscard]] UINT width() const { return m_width; }
		[[nodiscard]] UINT height() const { return m_height; }
		[[nodiscard]] DXGI_FORMAT format() const { return m_format; }
		[[nodiscard]] UINT current_backbuffer() { return m_swapchain->GetCurrentBackBufferIndex(); }
		[[nodiscard]] ID3D12Resource* render_target (const UINT index) const 
		{ 
			return m_render_targets[index].Get(); 
		}
		[[nodiscard]] UINT buffer_count() const { return m_buffer_counts; };

		void set_width(const int width);
		void set_height(const int height);
		void set_format(DXGI_FORMAT format) { m_format = format; }
	private:
		UINT m_width;
		UINT m_height;
		DXGI_FORMAT m_format;
		static constexpr UINT m_buffer_counts = 2; // Front + Back buffers

		ComPtr<IDXGISwapChain4> m_swapchain;
		std::array<ComPtr<ID3D12Resource>, m_buffer_counts> m_render_targets; // Double-buffering
	};
}