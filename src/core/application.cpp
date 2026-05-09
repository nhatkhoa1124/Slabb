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
		spdlog::trace("Initializing application's subsystems");

		load_toml_file("assets/config/application_cfg.toml");

		m_window->init(m_config.title, m_config.mode, m_config.width, 
					   m_config.height, m_config.resizable, m_config.visible);

		return true;
	}

	void Application::run()
	{
		while (!m_window->should_close())
		{

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
			auto toml = toml::parse_file(path);
			m_config.title = toml["application"]["window"]["title"].value_or("Undefined");
			spdlog::trace("Title: {}", m_config.title);

			const std::string mode_string = toml["application"]["window"]["mode"].value_or("");
			if (mode_string == "windowed")
			{
				m_config.mode = WindowMode::WINDOWED;
			}
			else if (mode_string == "fullscreen")
			{
				m_config.mode = WindowMode::FULLSCREEN;
			}
			else if (mode_string == "borderless_fullscreen")
			{
				m_config.mode = WindowMode::BORDERLESS_FULLSCREEN;
			}
			else
			{
				spdlog::warn("WARN: Invalid or no window mode config specified");
				m_config.mode = WindowMode::WINDOWED;
			}

			m_config.width = toml["application"]["window"]["width"].value_or(1280);
			m_config.height = toml["application"]["window"]["height"].value_or(720);
			m_config.resizable = toml["application"]["window"]["resizable"].value_or(false);
			m_config.visible = toml["application"]["window"]["visible"].value_or(false);

		}
		catch (const toml::parse_error& err)
		{
			spdlog::error("ERROR: Config file loading failed: {}", err.description());
		}
	}
}
