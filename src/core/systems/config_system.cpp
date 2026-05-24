#include "core/systems/config_system.hpp"
#include <toml++/toml.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace slabb::core::systems
{
	ConfigSystem::ConfigSystem(const std::string& file_path) :
		m_file_path {file_path}
	{}

	bool ConfigSystem::init_system()
	{
		spdlog::info("Loading configuration file: {}", m_file_path);
		// Check if file exists
		std::ifstream toml_file(m_file_path, std::ios::in);
		if (!toml_file)
		{
			spdlog::error("ERROR: Config file does not exists");
			return false;
		}
		toml_file.close();
		spdlog::info("Initializing config system...");
		load_app_config();
		load_renderer_config();
		spdlog::info("Config system initialized successfully");

		return true;
	}

	void ConfigSystem::load_app_config()
	{
		try
		{
			auto toml = toml::parse_file(m_file_path);
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
		}
		catch (const toml::parse_error& err)
		{
			spdlog::error("ERROR: Config file loading failed: {}", err.description());
		}
	}

	void ConfigSystem::load_renderer_config()
	{
		try
		{
			auto toml = toml::parse_file(m_file_path);
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