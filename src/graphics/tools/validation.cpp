#include "graphics/tools/validation.hpp"
#include <dxgidebug.h>
#include <d3d12sdklayers.h>

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::tools
{
	void Validation::enable_debug_layer()
	{
		ComPtr<ID3D12Debug> debug_controller;
		SLABB_CHECK(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)));
		debug_controller->EnableDebugLayer();
	}

	void Validation::set_gbv(bool enable)
	{
		ComPtr<ID3D12Debug1> debug_controller;
		debug_controller->SetEnableGPUBasedValidation(enable);
	}
}