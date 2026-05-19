#include "graphics/wrapper/shader_manager.hpp"

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::wrapper
{
	ShaderManager::ShaderManager()
	{}

	ComPtr<ID3DBlob> ShaderManager::compile_shader(const std::wstring& path, const std::string& entrypoint,
													const std::string& target)
	{
		if (path.empty())
		{
			spdlog::error("Shader path not found");
			throw std::runtime_error("ERROR: Invalid file path");
		}
		spdlog::info("Compiling shader: {}", std::string(path.begin(), path.end()));
		ComPtr<ID3DBlob> target_shader;
		ComPtr<ID3DBlob> error_blob;
		HRESULT hr = D3DCompileFromFile(path.c_str(), nullptr, nullptr, entrypoint.c_str(), target.c_str(),
									   m_compile_flags, 0, &target_shader, &error_blob);
		if (FAILED(hr))
		{
			if (error_blob)
			{
				const char* compile_errors = static_cast<const char*>(error_blob->GetBufferPointer());
				spdlog::error("Shader Compilation Failed:\n{}", compile_errors);
			}
			else
			{
				spdlog::error("Shader Compilation Failed");
			}

			SLABB_CHECK(hr);
		}
		spdlog::info("Shader compiled successfully");
		return target_shader;
	}
}