#pragma once
#include <memory>

#include "graphics/wrapper/window/window.hpp";

using slabb::graphics::wrapper::window::Window;

namespace slabb::core
{
	class Application {
	public:
		Application();
		~Application();
		void run();
	private:
		std::unique_ptr<Window> m_window;
	};
}
