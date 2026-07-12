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
		CD3DX12_DESCRIPTOR_RANGE1 ranges[1] = {};
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

		CD3DX12_ROOT_PARAMETER1 root_params[2] = {};
		root_params[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
		root_params[1].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

		D3D12_ROOT_SIGNATURE_FLAGS root_sig_flags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		CD3DX12_STATIC_SAMPLER_DESC linear_sampler(
			0,                                // Shader Register slot: s0
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,  // Filter behavior: Linear Blending interpolation
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // Address U: Wrap/Tile textures outside 0-1 range
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // Address V
			D3D12_TEXTURE_ADDRESS_MODE_WRAP   // Address W
		);
		linear_sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
		desc.Init_1_1(
			_countof(root_params),
			root_params,
			1,
			&linear_sampler,
			root_sig_flags
		);

		ComPtr<ID3DBlob> error_blob;
		spdlog::debug("Serializing root signature...");
		const auto result = D3D12SerializeVersionedRootSignature(&desc, &m_signature_blob, &error_blob);
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