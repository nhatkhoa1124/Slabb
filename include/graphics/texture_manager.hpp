#pragma once

#include "common/common_graphics.hpp"
#include <directx/d3d12.h>
#include <D3D12MemAlloc.h>
#include <array>
#include <string>
#include <vector>
#include <memory>

#include "graphics/render_graph.hpp"

using Microsoft::WRL::ComPtr;

namespace slabb::graphics::wrapper
{
	class Device;
}

namespace slabb::graphics::wrapper::command
{
	class CommandList;
}

namespace slabb::graphics
{
	class RenderGraph;

	/**
	* @brief Owns D3D12MA allocations and the per-frame upload ring for shader-bound textures.
	*
	* Centralizes texture creation so callers no longer thread device/allocator/cmd-list/graph
	* through every call site. The renderer constructs one of these in init_backend and calls
	* flush_uploads() once per frame, before any render pass executes.
	*/
	class SLABB_EXPORT TextureManager
	{
	public:
		TextureManager(wrapper::Device& device, RenderGraph& graph, UINT frame_count);
		~TextureManager();

		TextureManager(const TextureManager&) = delete;
		TextureManager& operator=(const TextureManager&) = delete;

		/**
		* @brief Allocate a default-heap RGBA8 texture and schedule an upload from CPU memory.
		*
		* The actual CopyTextureRegion + transition-to-SRV is recorded by flush_uploads() on the
		* caller's command list. The returned TextureResource is owned by the RenderGraph.
		*
		* @param debug_name  Human-readable name used for the graph resource and debug markers.
		* @param width       Texture width in texels.
		* @param height      Texture height in texels.
		* @param pixel_data  Pointer to width*height*4 bytes of RGBA8 pixel data (row-major, no padding).
		*/
		TextureResource* create_rgba8(const std::string& debug_name, UINT width, UINT height,
									  const void* pixel_data);

		/**
		* @brief Allocate a committed depth-stencil buffer (D24_UNORM_S8_UINT).
		*
		* Mirrors the previous TextureResource::initialize_hardware_depth behavior exactly: goes
		* through device->CreateCommittedResource rather than D3D12MA. The returned resource is
		* owned by the RenderGraph.
		*/
		TextureResource* create_depth(const std::string& debug_name, UINT width, UINT height);

		// Native device is cached at construction time so the inline accessors below don't need
		// the full wrapper::Device definition in this header.
		[[nodiscard]] ID3D12Device* device() noexcept { return m_native_device; }
		[[nodiscard]] RenderGraph& graph() noexcept { return m_graph; }

		/**
		* @brief Register an externally-owned texture (e.g. swapchain backbuffer) with the graph.
		*
		* No allocation, no upload, no lifetime transfer. The native resource's owner must keep
		* it alive as long as the graph reference exists.
		*/
		TextureResource* import_external(const std::string& debug_name, ID3D12Resource* native_resource,
										 TextureUsage usage);

		/**
		* @brief Record any pending texture uploads onto the given command list.
		*
		* Must be called once per frame, after Reset() and before any render pass executes, on
		* the same command list that will perform subsequent draw work. Upload resources are kept
		* alive in a per-frame ring; the caller rotates the ring via set_current_frame().
		*/
		void flush_uploads(wrapper::command::CommandList& cmd_list);

		/**
		* @brief Tell the manager which frame index is being recorded this frame.
		*
		* The previous frame's staging ring is reclaimed on the next call. Renderer calls this
		* at the top of render_frame.
		*/
		void set_current_frame(UINT frame_index);

	private:
		struct PendingCopy
		{
			ID3D12Resource* dst{ nullptr };            // owned by the graph's TextureResource
			ComPtr<ID3D12Resource> src;                // upload-heap staging buffer
			ComPtr<D3D12MA::Allocation> src_alloc;     // upload-heap allocation
			ComPtr<D3D12MA::Allocation> dst_alloc;     // default-heap destination allocation
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
		};

		std::vector<PendingCopy>& ring_for(UINT frame_index)
		{
			return m_pending[frame_index % m_pending.size()];
		}

		wrapper::Device& m_device;
		RenderGraph& m_graph;
		ID3D12Device* m_native_device{ nullptr }; // cached; mirrors m_device.device()
		UINT m_frame_count{};
		UINT m_current_frame{0};

		// Per-frame staging ring. Each frame slot owns an inner vector of pending copies, so a
		// frame can submit any number of texture uploads in a single pass.
		std::vector<std::vector<PendingCopy>> m_pending;
	};
}