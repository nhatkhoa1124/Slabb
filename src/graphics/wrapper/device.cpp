#include "graphics/wrapper/device.hpp"
#include <directx/d3d12.h>

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::wrapper
{
	Device::Device() 
	{
		m_instance = std::make_unique<Instance>();
	}

	void Device::create_device()
	{
		NULL_CHECK(m_instance);
		m_instance->create_factory();
		m_instance->enumerate_adapter();
		spdlog::trace("Creating D3D12 device");
		SLABB_CHECK(D3D12CreateDevice(m_instance->adapter(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
	}
}
