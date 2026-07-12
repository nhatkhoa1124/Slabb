#pragma once
#include "common/common_graphics.hpp"
#include <directx/d3d12.h>

#include "render_graph.hpp"
#include "context.hpp"
#include "render_queue.hpp"
#include "graphics_interface/graphics_shader.hpp"
#include "graphics_interface/graphics_vertex.hpp"

namespace slabb::graphics
{
	/**
	* @brief Holds scene information, is a core-graphics interface
	*/
	class SLABB_EXPORT Scene
	{
	public:
		Scene() = default;

		void load_model(const GraphicsModel& model, RenderGraph& render_graph, ID3D12Device* device);
		void update_transforms(const FrameContext& frame);

		/**
		* @brief Flattens render queue into a linear array
		*/
		void collect_render_items();

		[[nodiscard]] const std::vector<RenderModel>& models() const { return m_models; }
		[[nodiscard]] const RenderQueue& render_queue() const noexcept { return m_render_queue; }

	private:
		std::vector<RenderModel> m_models;
		RenderQueue m_render_queue;
		float m_rotation_angle{ 0.0f };
	};
}