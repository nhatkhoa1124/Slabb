#pragma once
#include "common/common.hpp"
#include <memory>
#include <vector>

#include "config_system.hpp"
#include "graphics/renderer.hpp"
#include "../model/model.hpp"

using slabb::graphics::Renderer;

class Model;

namespace slabb::core::systems
{
	class SLABB_EXPORT RenderSystem
	{
	public:
		RenderSystem();
		bool init_system(const ConfigSystem& config, HWND window_handle);
		void run();
		void cleanup();
		void load_model(const core::model::Model& model);

		[[nodiscard]] inline Renderer* renderer() { return m_renderer.get(); }
	private:
		void draw_mesh(std::vector<Model> models);
	private:
		std::unique_ptr<Renderer> m_renderer;
	};
}