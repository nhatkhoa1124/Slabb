#include "graphics/wrapper/instance.hpp"

#include "graphics/tools/validation.hpp"
#include "graphics/tools/debug.hpp"
#include "graphics/tools/string_utils.hpp"

using slabb::graphics::tools::Validation;
using slabb::graphics::tools::StringUtils;

namespace slabb::graphics::wrapper
{
	void Instance::create_factory()
	{
		spdlog::info("Creating DXGI factory...");
		UINT debug_flag = 0;
#ifdef _DEBUG
		spdlog::info("Enabling debug layer...");
		Validation validation = {};
		validation.enable_debug_layer();
		debug_flag |= DXGI_CREATE_FACTORY_DEBUG;
		spdlog::info("Debug layer enabled successfully");
#endif
		SLABB_CHECK(CreateDXGIFactory2(debug_flag, IID_PPV_ARGS(&m_factory)));
		spdlog::info("Factory created successfully");
	}

	void Instance::enumerate_adapter()
	{
		UINT adapter_index = 0;
		UINT max_vram = 0;
		ComPtr<IDXGIAdapter1> adapter, best_adapter;
		std::string best_adapter_name;
		spdlog::info("Enumerating adapters...");
		for (size_t adapter_index = 0; m_factory->EnumAdapters1(adapter_index, &adapter) != DXGI_ERROR_NOT_FOUND;
			adapter_index++)
		{
			DXGI_ADAPTER_DESC1 desc;
			SLABB_CHECK(adapter->GetDesc1(&desc));

			// spdlog does not take wchar
			std::string desc_string = StringUtils::convert_wchar_to_string(desc.Description);
			spdlog::info("Found Adapter: [{}], {}", adapter_index, desc_string);
	
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				spdlog::debug("Skipping software adapter");
				continue;
			}
			if (desc.DedicatedVideoMemory > max_vram)
			{
				best_adapter = adapter;
				best_adapter_name = desc_string;
				max_vram = desc.DedicatedVideoMemory;
			}
		}
		spdlog::debug("Choosing adapter: {}", best_adapter_name);
		// For modern features support
		SLABB_CHECK(best_adapter.As(&m_adapter)); 
		spdlog::info("Adapter enumerated successfully");
	}
}