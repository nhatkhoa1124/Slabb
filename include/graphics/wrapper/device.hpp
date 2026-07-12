#pragma once
#include "common/common_graphics.hpp"
#include <dxgi1_6.h>
#include <D3D12MemAlloc.h>
#include <memory>

using Microsoft::WRL::ComPtr;

class ID3D12Device;

namespace slabb::graphics::wrapper
{
	class SLABB_EXPORT Device {
	public:
		Device();
		/**
		* @brief Create D3D12 virtual device
		*/
		void create_device(IDXGIAdapter* adapter);

		[[nodiscard]] ID3D12Device* device() const { return m_device.Get(); }
		[[nodiscard]] D3D12MA::Allocator* allocator() const { return m_allocator.Get(); }
	private:
		ComPtr<ID3D12Device> m_device;
		ComPtr<D3D12MA::Allocator> m_allocator;
	};
}
