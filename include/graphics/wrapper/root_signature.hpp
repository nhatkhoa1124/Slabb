#pragma once
#include "common/common_graphics.hpp"
#include <directx/d3d12.h>

using Microsoft::WRL::ComPtr;

namespace slabb::graphics::wrapper
{
	class SLABB_EXPORT RootSignature
	{
	public:
		RootSignature();
		void create_root_signature(ID3D12Device* device);
		bool serialize_root_signature();
		[[nodiscard]] ID3D12RootSignature* root_signature() const { return m_root_signature.Get(); }
	private:
		ComPtr<ID3D12RootSignature> m_root_signature;
		ComPtr<ID3DBlob> m_signature_blob;
	};
}