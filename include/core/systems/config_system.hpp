#pragma once 
#include "common/common.hpp"
#include <string>
#include <vector>

#include "graphics/wrapper/window/window.hpp"

namespace slabb::core::systems
{
	struct AppConfig {
		// Window settings
		std::string title;
		WindowMode mode;
		int width;
		int height;
		bool resizable;
		bool visible;
	};

	struct RendererConfig
	{
		// Shader file paths
		std::vector<std::string> vertex_files;
		std::vector<std::string> pixel_files;
	};

	class SLABB_EXPORT ConfigSystem
	{
	public:
		ConfigSystem(const std::string& file_path);
		bool init_system();
		void load_app_config();
		void load_renderer_config();

		[[nodiscard]] inline const AppConfig& app_config() const { return m_app_config; }
		[[nodiscard]] inline const RendererConfig& renderer_config() const {return m_renderer_config;}
	private:
		AppConfig m_app_config;
		RendererConfig m_renderer_config;
		std::string m_file_path;
	};
}