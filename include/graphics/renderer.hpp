#pragma once
#include <memory>

#include "render_graph.hpp"

namespace slabb::graphics
{
	class __declspec(dllexport) Renderer {
	public:
		Renderer();
		void render_frame();
	private:
		std::unique_ptr<RenderGraph> m_render_graph;
	};
}

