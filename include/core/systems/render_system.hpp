#pragma once
#include "common/common.hpp"
#include <memory>
#include <vector>

#include "config_system.hpp"
#include "graphics/renderer.hpp"
#include "../model/model.hpp"

using slabb::graphics::Renderer;

namespace slabb::graphics
{
	class Scene;
}

namespace slabb::core::systems
{
	class SLABB_EXPORT RenderSystem
	{
	public:
		RenderSystem();
		bool init_system(const ConfigSystem& config, HWND window_handle);
		void run(slabb::graphics::Scene& active_scene);
		void cleanup();
		void load_model(const core::model::Model& model, slabb::graphics::Scene& target_scene);

		[[nodiscard]] inline Renderer* renderer() { return m_renderer.get(); }
	private:
		std::unique_ptr<Renderer> m_renderer;
	};
}