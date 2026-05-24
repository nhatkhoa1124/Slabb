#pragma once
#include "common/common.hpp"
#include <memory>

#include "config_system.hpp"
#include "graphics/renderer.hpp"

using slabb::graphics::Renderer;

namespace slabb::core::systems
{
	class SLABB_EXPORT RenderSystem
	{
	public:
		RenderSystem();
		bool init_system(const ConfigSystem& config, HWND window_handle);
		void run();
		void cleanup();

		[[nodiscard]] inline Renderer* renderer() { return m_renderer.get(); }
	private:
		std::unique_ptr<Renderer> m_renderer;
	};
}