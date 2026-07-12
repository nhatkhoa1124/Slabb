#include "core/systems/render_system.hpp"
#include <spdlog/spdlog.h>

#include "graphics/graphics_interface/graphics_vertex.hpp"
#include "graphics/scene.hpp"
#include "graphics/wrapper/device.hpp"
#include "graphics/wrapper/resource/descriptor_heap.hpp"
#include "graphics/render_graph.hpp"
#include "graphics/texture_manager.hpp"

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
		m_renderer->init_pipeline(default_vs, default_ps, vertexAttributes);
		spdlog::info("Config system initialized successfully");
		return true;
	}

	void RenderSystem::run(slabb::graphics::Scene& active_scene)
	{
		// render_frame handles collect_render_items() itself so the descriptor heap base is
		// captured on the queue right before dispatch.
		m_renderer->render_frame(active_scene);
	}

	void RenderSystem::cleanup()
	{

	}

	void RenderSystem::load_model(const core::model::Model& model, slabb::graphics::Scene& target_scene)
	{
		slabb::graphics::GraphicsModel graphics_model;
		graphics_model.transform = DirectX::XMMatrixIdentity();

		// Forward images alongside meshes. Each mesh's base_color_texture references an entry
		// in this vector.
		graphics_model.images.reserve(model.images.size());
		for (const auto& img : model.images)
		{
			slabb::graphics::TextureImage gimg;
			gimg.width = img.width;
			gimg.height = img.height;
			gimg.pixels = img.pixels;
			graphics_model.images.push_back(std::move(gimg));
		}

		for (size_t i = 0; i < model.meshes.size(); ++i)
		{
			const auto& core_mesh = model.meshes[i];
			slabb::graphics::GraphicsMesh raw_mesh;
			raw_mesh.vertex_data = core_mesh.vertices.data();
			raw_mesh.vertex_count = core_mesh.vertices.size();
			raw_mesh.vertex_stride = sizeof(slabb::core::Vertex);

			raw_mesh.index_data = core_mesh.indices.empty() ? nullptr : core_mesh.indices.data();
			raw_mesh.index_count = core_mesh.indices.size();

			raw_mesh.base_color_texture = core_mesh.base_color_texture;

			graphics_model.meshes.push_back(raw_mesh);
		}

		// Bind scene->runtime caches (texture manager + descriptor heap for SRV allocation) the
		// first time we load. Subsequent loads reuse the bindings.
		static bool bound_once = false;
		if (!bound_once)
		{
			target_scene.bind_runtime(*m_renderer->texture_manager(),
									  *m_renderer->descriptor_heap());
			bound_once = true;
		}

		target_scene.load_model(graphics_model);

		// Now that we know the textures, build the render passes against the scene. Calling this
		// again on every load would re-add duplicate passes; for now, only build after the first
		// model.
		static bool passes_built_once = false;
		if (!passes_built_once)
		{
			m_renderer->setup_render_passes(target_scene);
			passes_built_once = true;
		}
	}
}