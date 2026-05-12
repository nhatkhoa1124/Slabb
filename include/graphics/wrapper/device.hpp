#pragma once
#include "common/common_graphics.hpp"
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

namespace slabb::graphics::wrapper
{
	class SLABB_EXPORT Device {
	public:
		Device() = default;
		void create_device(IUnknown* pAdapter, D3D_FEATURE_LEVEL min_feature_level, REFIID id, void** ppDevice);

		inline ID3D12Device* device() const { return m_device.Get(); }
	private:
		ComPtr<ID3D12Device> m_device;
	};
}
