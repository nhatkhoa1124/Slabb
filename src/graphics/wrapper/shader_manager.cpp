#include "graphics/wrapper/shader_manager.hpp"

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::wrapper
{
	ShaderManager::ShaderManager()
	{}

	ComPtr<ID3DBlob> ShaderManager::compile_shaders(const std::wstring& path, const std::string& entrypoint,
													const std::string& target)
	{
		if (path.empty())
		{
			spdlog::error("Shader path not found");
			throw std::runtime_error("ERROR: Invalid file path");
		}
		spdlog::info("Compiling shader: {}", std::string(path.begin(), path.end()));
		ComPtr<ID3DBlob> target_shader;
		SLABB_CHECK(D3DCompileFromFile(path.c_str(), nullptr, nullptr, entrypoint.c_str(), target.c_str(),
									   m_compile_flags, 0, &target_shader, nullptr));
		spdlog::info("Shader compiled successfully");
		return target_shader;
	}
}