#include "core/application.hpp"

#include <toml++/toml.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

#include "graphics/scene.hpp"

namespace slabb::core
{
	Application::Application()
	{
		m_config_system = std::make_unique<ConfigSystem>("assets/config/application_cfg.toml");
		m_render_system = std::make_unique<RenderSystem>();
		m_window = std::make_unique<Window>();
		m_active_scene = std::make_unique <slabb::graphics::Scene>();
	}

	Application::~Application() 
	{

	}

	bool Application::init_subsystems()
	{
		spdlog::info("Initializing application's subsystems...");
		// Load configuration
		m_config_system->init_system();
		// Sub-system initialization
		m_window->init(m_config_system->app_config().title, m_config_system->app_config().mode,
			m_config_system->app_config().width, m_config_system->app_config().height,
			m_config_system->app_config().resizable, m_config_system->app_config().visible);
		m_render_system->init_system(*m_config_system, m_window->get_native_handle());

		// Hard-coded mesh data first!!!
		slabb::core::model::Mesh square_mesh;
		square_mesh.vertices = {
			// 0: Bottom-Left (Red)
			{ { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
			// 1: Top-Left (Green)
			{ { -0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
			// 2: Top-Right (Blue)
			{ {  0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
			// 3: Bottom-Right (White)
			{ {  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } }
		};
		square_mesh.indices = {
			0, 1, 2, // First Triangle
			0, 2, 3  // Second Triangle
		};

		slabb::core::model::Model bike_model = slabb::core::model::ModelLoader::load_gltf("assets/models/bike.glb");
		m_render_system->load_model(bike_model, *m_active_scene);

		spdlog::info("Application subsystems initialized successfully");
		return true;
	}

	void Application::run()
	{
		while (!m_window->should_close())
		{
			m_window->poll();
			m_render_system->run(*m_active_scene);
		}
	}
}
