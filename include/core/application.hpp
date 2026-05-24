#pragma once
#include "common/common.hpp"
#include <memory>
#include <string>
#include <vector>

#include "systems/config_system.hpp"
#include "systems/render_system.hpp"
#include "graphics/wrapper/window/window.hpp"

using slabb::core::systems::ConfigSystem;
using slabb::core::systems::RenderSystem;
using slabb::graphics::wrapper::window::Window;

namespace slabb::core
{
	class SLABB_EXPORT Application {
	public:
		Application();
		~Application();
		bool init_subsystems();
		void run();
	private:
		std::unique_ptr<Window> m_window;
		std::unique_ptr<ConfigSystem> m_config_system;
		std::unique_ptr<RenderSystem> m_render_system;
	};
}
