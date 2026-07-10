#include "core/systems/render_system.hpp"
#include <spdlog/spdlog.h>
#include <DirectXMath.h>

#include "graphics/graphics_interface/graphics_vertex.hpp"

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

		std::vector<VertexAttribute> vertexAttributes = core::Vertex::vertex_attribute();
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

	void RenderSystem::load_model(const core::model::Model& model)
	{
		slabb::graphics::GraphicsModel graphics_model;
		graphics_model.transform = DirectX::XMMatrixIdentity();

		for (const auto& core_mesh : model.meshes)
		{
			slabb::graphics::GraphicsMesh raw_mesh;
			raw_mesh.vertex_data = core_mesh.vertices.data();
			raw_mesh.vertex_count = core_mesh.vertices.size();
			raw_mesh.vertex_stride = sizeof(slabb::core::Vertex);

			raw_mesh.index_data = core_mesh.indices.empty() ? nullptr : core_mesh.indices.data();
			raw_mesh.index_count = core_mesh.indices.size();

			graphics_model.meshes.push_back(raw_mesh);
		}
		m_renderer->load_model(graphics_model);
	}
}