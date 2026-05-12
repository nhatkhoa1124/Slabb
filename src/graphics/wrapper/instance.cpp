#include "graphics/wrapper/instance.hpp"

#include "graphics/tools/validation.hpp"
#include "graphics/tools/debug.hpp"

using slabb::graphics::tools::Validation;

namespace slabb::graphics::wrapper
{
	void Instance::create_factory()
	{
		UINT debug_flag = 0;
#ifdef _DEBUG
		Validation validation = {};
		validation.enable_debug_layer();
		debug_flag |= DXGI_CREATE_FACTORY_DEBUG;
#endif
		SLABB_CHECK(CreateDXGIFactory2(debug_flag, IID_PPV_ARGS(&m_factory)));
	}

	void Instance::enumerate_adapters()
	{
		UINT adapter_index = 0;
		UINT max_vram = 0;
		ComPtr<IDXGIAdapter1> adapter, best_adapter;
		while (m_factory->EnumAdapters1(adapter_index, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 desc;
			SLABB_CHECK(adapter->GetDesc1(&desc));
			spdlog::info("Found Adapter: [{}], {}", adapter_index, desc.Description);
			// No support for software renderer
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
			// Query gpu by most VRAM
			if (desc.DedicatedVideoMemory > max_vram)
			{
				best_adapter = adapter;
				max_vram = desc.DedicatedVideoMemory;
			}

			adapter_index++;
		}
		// For modern features support
		SLABB_CHECK(best_adapter.As(&m_adapter)); 
	}
}