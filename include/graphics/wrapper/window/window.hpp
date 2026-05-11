#pragma once
#include "common/common.hpp"
#include <string>

struct GLFWwindow;

enum class WindowMode{FULLSCREEN, WINDOWED, BORDERLESS_FULLSCREEN};

namespace slabb::graphics::wrapper::window
{
	class SLABB_EXPORT Window
	{
	public:
		Window();
		bool init(const std::string& title, WindowMode mode, const int width, 
				  const int height, const bool resizable, const bool visible);
		void set_cursor_mode();
		void set_user_pointer();
		void set_title(const std::string& title);
		void poll();
		void close_window();
		void show();
		bool should_close();
		void capture_cursor();
		void cleanup();

		[[nodiscard]] inline GLFWwindow* window() const { return m_window; }
		[[nodiscard]] inline std::string title() const { return m_title; }
		[[nodiscard]] inline int width() const { return m_width; }
		[[nodiscard]] inline int height() const { return m_height; }
	private:
		GLFWwindow* m_window;
		WindowMode m_mode;
		std::string m_title;
		int m_width;
		int m_height;
	};
}

