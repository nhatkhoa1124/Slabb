#include "graphics/wrapper/root_signature.hpp"
#include <directx/d3dx12.h>

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::wrapper
{
	RootSignature::RootSignature()
	{

	}

	void RootSignature::create_root_signature(ID3D12Device* device)
	{
		NULL_CHECK(device);
		spdlog::info("Creating root signature...");
		if (serialize_root_signature())
		{
			SLABB_CHECK(device->CreateRootSignature(0, m_signature_blob->GetBufferPointer(), 
						m_signature_blob->GetBufferSize(), IID_PPV_ARGS(&m_root_signature)));
		}
		else
		{
			spdlog::error("Unable to create root signature");
			return;
		}

		spdlog::info("Root signature created successfully");
	}

	bool RootSignature::serialize_root_signature()
	{
		CD3DX12_ROOT_SIGNATURE_DESC desc;
		desc.Init(
			0,
			nullptr,
			0,
			nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		);

		ComPtr<ID3DBlob> error_blob;
		spdlog::debug("Serializing root signature...");
		const auto result = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, 
														&m_signature_blob, &error_blob);
		if (FAILED(result))
		{
			if (error_blob != nullptr)
			{
				spdlog::error("Root signature serialization failed: {}",
					static_cast<const char*>(error_blob->GetBufferPointer()));
			}
			else
			{
				spdlog::error("Root signature serialization failed without info");
			}
			return false;
		}

		spdlog::debug("Root signature serialized successfully");
		return true;
	}
}