#include "graphics/wrapper/window/window.hpp"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <cassert>
#include <stdexcept>

namespace slabb::graphics::wrapper::window {
	Window::Window() : 
		m_window{ nullptr }, m_title{""}, m_width{0}, m_height{0}
	{}

	bool Window::init(const std::string& title, WindowMode mode, const int width,
					  const int height, const bool resizable, const bool visible) 
	{
		assert(!title.empty());
		// Assign member values
		m_title = title;
		m_width = width;
		m_height = height;
		m_mode = mode;

		if (!glfwInit())
		{
			throw std::runtime_error("ERROR: Failed to initialize GLFW");
		}
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
		glfwWindowHint(GLFW_VISIBLE, visible ? GLFW_TRUE : GLFW_FALSE);
		
		GLFWmonitor* monitor = nullptr;
		if (m_mode != WindowMode::WINDOWED)
		{
			monitor = glfwGetPrimaryMonitor();
			if (m_mode == WindowMode::BORDERLESS_FULLSCREEN)
			{
				const auto* video = glfwGetVideoMode(monitor);
				m_width = video->width;
				m_height = video->height;
			}
		}

		spdlog::trace("Creating window...");
		m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), monitor, nullptr);
		if (!m_window)
		{
			throw std::runtime_error("ERROR: Failed to create window");
		}

		return true;
	}
	
	void Window::set_title(const std::string& title)
	{
		assert(!title.empty());
		m_title = title;
		glfwSetWindowTitle(m_window, m_title.c_str());
	}

	void Window::poll()
	{
		glfwPollEvents();
	}

	void Window::capture_cursor()
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void Window::show()
	{
		glfwShowWindow(m_window);
	}

	void Window::close_window()
	{
		glfwSetWindowShouldClose(m_window, GLFW_TRUE);
	}

	bool Window::should_close()
	{
		return glfwWindowShouldClose(m_window) == GLFW_TRUE;
	}

	void Window::cleanup() {
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}
}