#pragma once
#include <memory>
#include <string>

#include "graphics/wrapper/window/window.hpp"

using slabb::graphics::wrapper::window::Window;

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

	class __declspec(dllexport) Application {
	public:
		Application();
		~Application();
		bool init_subsystems();
		void run();
		void load_toml_file(const std::string& path);
	private:
		AppConfig m_config{};

		std::unique_ptr<Window> m_window;
	};
}
