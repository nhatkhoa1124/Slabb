#pragma once
#include "common/common_graphics.hpp"
#include <dxgi1_6.h>
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
	private:
		ComPtr<ID3D12Device> m_device;
	};
}
