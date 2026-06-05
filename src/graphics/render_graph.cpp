#include "graphics/render_graph.hpp"

namespace slabb::graphics
{
	RenderGraph::RenderGraph(int width, int height)
	{
		D3D12_VIEWPORT viewport = {};
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		m_render_resource.viewport = viewport;

		D3D12_RECT scissor_rect = {};
		scissor_rect.left = 0;
		scissor_rect.top = 0;
		scissor_rect.right = static_cast<LONG>(width);
		scissor_rect.bottom = static_cast<LONG>(height);
		m_render_resource.rect = scissor_rect;
	}
}