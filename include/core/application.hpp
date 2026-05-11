#pragma once
#include "common/common.hpp"
#include <memory>
#include <string>

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

	class SLABB_EXPORT Application {
	public:
		Application();
		~Application();
		bool init_subsystems();
		void run();
		void load_toml_file(const std::string& path);
	private:
		AppConfig m_config{};

		std::unique_ptr<Window> m_window;
		std::unique_ptr<Renderer> m_renderer;
	};
}
