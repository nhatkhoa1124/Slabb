#include "core/systems/render_system.hpp"

#include <spdlog/spdlog.h>

namespace slabb::core::systems
{
	RenderSystem::RenderSystem()
	{

	}

	bool RenderSystem::init_system(const ConfigSystem& config, HWND window_handle)
	{
		// Add config dependency
		spdlog::info("Initializing config system...");
		// Create renderer
		m_renderer = std::make_unique<Renderer>(config.app_config().width, config.app_config().height);
		m_renderer->init_backend(window_handle);
		// Hard-coding this !!!!!!
		std::vector<VertexAttribute> vertexAttributes =
		{
			VertexAttribute::Position,
			VertexAttribute::Color
		};
		const std::string& default_vs = config.renderer_config().vertex_files[0];
		const std::string& default_ps = config.renderer_config().pixel_files[0];
		m_renderer->init_default_pipeline(default_vs, default_ps, vertexAttributes);
		spdlog::info("Config system initialized successfully");
		return true;
	}

	void RenderSystem::run()
	{
		m_renderer->render_frame();
	}

	void RenderSystem::cleanup()
	{

	}
}