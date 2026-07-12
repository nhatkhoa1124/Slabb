#pragma once
#include <vector>
#include <DirectXMath.h>
#include <directx/d3d12.h>

namespace slabb::graphics
{
	class BufferResource;
	class TextureResource;

	struct RenderItem
	{
		BufferResource* vertex_buffer{ nullptr };
		BufferResource* index_buffer{ nullptr };
		uint32_t index_count{ 0 };
		uint32_t vertex_count{ 0 };
		DirectX::XMMATRIX world_matrix; // Per-object placement payload

		// GPU descriptor handle for this draw's base-color SRV (slot on the resource heap). A
		// null handle means "no texture; pixel shader should fall back to vertex color".
		D3D12_GPU_DESCRIPTOR_HANDLE srv_gpu_handle{};
	};

	struct RenderQueue
	{
		std::vector<RenderItem> opaque_buckets;
		std::vector<RenderItem> transparent_buckets;
		std::vector<RenderItem> shadow_casters;

		// Base GPU handle of the resource heap captured at queue build time, so the render
		// pass can offset by srv_slot * descriptor_size without re-querying every draw.
		D3D12_GPU_DESCRIPTOR_HANDLE srv_heap_base{};
		UINT srv_descriptor_size{ 0 };

		void clear() noexcept
		{
			opaque_buckets.clear();
			transparent_buckets.clear();
			shadow_casters.clear();
		}
	};

}