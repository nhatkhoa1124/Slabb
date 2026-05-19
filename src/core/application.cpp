#include "core/application.hpp"

#include <toml++/toml.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace slabb::core
{
	Application::Application()
	{
		m_window = std::make_unique<Window>();
	}

	Application::~Application() 
	{

	}

	bool Application::init_subsystems()
	{
		spdlog::info("Initializing application's subsystems...");
		// Start application systems
		load_toml_file("assets/config/application_cfg.toml");
		// Initialize subsystems
		m_window->init(m_app_config.title, m_app_config.mode, m_app_config.width,
					   m_app_config.height, m_app_config.resizable, m_app_config.visible);
		m_renderer = std::make_unique<Renderer>(m_app_config.width, m_app_config.height);
		m_renderer->init_backend(m_window->get_native_handle());
		// Hard-coding this !!!!!!
		std::vector<VertexAttribute> vertexAttributes = 
		{
			VertexAttribute::Position,
			VertexAttribute::Color
		};
		const std::string& default_vs = m_renderer_config.vertex_files[0];
		const std::string& default_ps = m_renderer_config.pixel_files[0];
		m_renderer->init_default_pipeline(default_vs, default_ps,vertexAttributes);

		return true;
	}

	void Application::run()
	{
		while (!m_window->should_close())
		{
			m_window->poll();
			m_renderer->render_frame();
		}
	}

	void Application::load_toml_file(const std::string& path)
	{
		spdlog::trace("Loading config file: {}", path);
		// Check if file exists first
		std::ifstream toml_file(path, std::ios::in);
		if (!toml_file)
		{
			throw std::runtime_error("ERROR: File does not exists");
		}
		toml_file.close();

		try
		{
			// App config
			auto toml = toml::parse_file(path);
			m_app_config.title = toml["application"]["window"]["title"].value_or("Undefined");
			spdlog::trace("Title: {}", m_app_config.title);

			const std::string mode_string = toml["application"]["window"]["mode"].value_or("");
			if (mode_string == "windowed")
			{
				m_app_config.mode = WindowMode::WINDOWED;
			}
			else if (mode_string == "fullscreen")
			{
				m_app_config.mode = WindowMode::FULLSCREEN;
			}
			else if (mode_string == "borderless_fullscreen")
			{
				m_app_config.mode = WindowMode::BORDERLESS_FULLSCREEN;
			}
			else
			{
				spdlog::warn("WARN: Invalid or no window mode config specified");
				m_app_config.mode = WindowMode::WINDOWED;
			}

			m_app_config.width = toml["application"]["window"]["width"].value_or(1280);
			m_app_config.height = toml["application"]["window"]["height"].value_or(720);
			m_app_config.resizable = toml["application"]["window"]["resizable"].value_or(false);
			m_app_config.visible = toml["application"]["window"]["visible"].value_or(false);

			// Renderer config
			const auto vertex_paths = toml["renderer"]["shader"]["vertex_files"].as_array();
			for (const auto& value : *vertex_paths)
			{
				const std::string file_string = value.value_or("");
				spdlog::trace("VS File - {}", file_string);
				m_renderer_config.vertex_files.push_back(file_string);
			}

			const auto pixel_paths = toml["renderer"]["shader"]["pixel_files"].as_array();
			for (const auto& value : *pixel_paths)
			{
				const std::string file_string = value.value_or("");
				spdlog::trace("PS File - {}", file_string);
				m_renderer_config.pixel_files.push_back(file_string);
			}

		}
		catch (const toml::parse_error& err)
		{
			spdlog::error("ERROR: Config file loading failed: {}", err.description());
		}
	}
}
