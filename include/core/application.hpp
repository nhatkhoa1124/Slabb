#pragma once
#include "common/common.hpp"
#include <memory>
#include <string>
#include <vector>

#include "graphics/wrapper/window/window.hpp"
#include "graphics/renderer.hpp"

using slabb::graphics::wrapper::window::Window;
using slabb::graphics::Renderer;

namespace slabb::core
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

	class SLABB_EXPORT Application {
	public:
		Application();
		~Application();
		bool init_subsystems();
		void run();
		void load_toml_file(const std::string& path);
	private:
		AppConfig m_app_config{};
		RendererConfig m_renderer_config{};

		std::unique_ptr<Window> m_window;
		std::unique_ptr<Renderer> m_renderer;
	};
}
