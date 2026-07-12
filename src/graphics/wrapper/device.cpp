#include "graphics/wrapper/device.hpp"
#include <directx/d3d12.h>

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::wrapper
{
	Device::Device() 
	{
	}

	void Device::create_device(IDXGIAdapter* adapter)
	{
		spdlog::info("Creating device...");
		NULL_CHECK(adapter);
		SLABB_CHECK(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

		D3D12MA::ALLOCATOR_DESC allocator_desc = {};
		allocator_desc.pDevice = m_device.Get();
		allocator_desc.pAdapter = adapter;
		SLABB_CHECK(D3D12MA::CreateAllocator(&allocator_desc, &m_allocator));

		spdlog::info("Device created successfully");
	}
}
