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
	class TextureManager;
	namespace wrapper::descriptor { class DescriptorHeap; }

	/**
	* @brief Holds scene information, is a core-graphics interface
	*/
	class SLABB_EXPORT Scene
	{
	public:
		Scene() = default;

		/**
		* @brief Cache resource references used during model loading and draw dispatch.
		* Call once, after Renderer::init_backend, before any load_model.
		*/
		void bind_runtime(TextureManager& texture_manager,
						  wrapper::descriptor::DescriptorHeap& resource_heap);

		/**
		* @brief Build GPU resources for a model and register them with the render graph.
		*
		* Textures in the model are uploaded through TextureManager; SRV descriptors are allocated
		* from the resource heap, starting from the slot cursor maintained here.
		*/
		void load_model(const GraphicsModel& model);
		void update_transforms(const FrameContext& frame);

		/**
		* @brief Flattens render queue into a linear array
		*/
		void collect_render_items();

		[[nodiscard]] const std::vector<RenderModel>& models() const { return m_models; }
		[[nodiscard]] const RenderQueue& render_queue() const noexcept { return m_render_queue; }

		[[nodiscard]] const std::vector<TextureResource*>& textures() const noexcept { return m_textures; }

	private:
		std::vector<RenderModel> m_models;
		std::vector<TextureResource*> m_textures; // every SRV-bound texture, one per dedup'd image
		RenderQueue m_render_queue;
		float m_rotation_angle{ 0.0f };

		TextureManager* m_texture_manager{ nullptr };
		wrapper::descriptor::DescriptorHeap* m_resource_heap{ nullptr };
		UINT m_next_srv_slot{ 0 };
	};
}