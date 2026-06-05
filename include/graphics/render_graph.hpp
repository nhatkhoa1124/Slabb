#pragma once
#include "common/common_graphics.hpp"
#include <directx/d3d12.h>
#include "wrapper/instance.hpp"

namespace slabb::graphics
{
	struct RenderResource
	{
		D3D12_VIEWPORT viewport;
		D3D12_RECT rect;
	};

	class SLABB_EXPORT RenderGraph {
	public:
		RenderGraph(int width, int height);

		[[nodiscard]] inline const RenderResource& render_resource() const { return m_render_resource; }
	private:
		RenderResource m_render_resource;
	};
}
