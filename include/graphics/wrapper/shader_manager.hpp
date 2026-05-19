#pragma once
#include "common/common_graphics.hpp"
#include <directx/d3d12.h>
#include <d3dcompiler.h>
#include <string>

using Microsoft::WRL::ComPtr;

namespace slabb::graphics::wrapper
{
	/**
	* @brief Manager class for shaders
	*/
	class SLABB_EXPORT ShaderManager
	{
	public:
		ShaderManager();
		[[nodiscard]] static ComPtr<ID3DBlob> compile_shader(const std::wstring& path, const std::string& entrypoint,
															  const std::string& target);
	private:

#if defined(_DEBUG)
		static constexpr UINT m_compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		static constexpr UINT m_compile_flags = 0;
#endif
	};

}