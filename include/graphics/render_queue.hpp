#pragma once
#include <vector>
#include <DirectXMath.h>

namespace slabb::graphics
{
	class BufferResource;

	struct RenderItem
	{
		BufferResource* vertex_buffer{ nullptr };
		BufferResource* index_buffer{ nullptr };
		uint32_t index_count{ 0 };
		uint32_t vertex_count{ 0 };
		DirectX::XMMATRIX world_matrix; // Per-object placement payload
	};

	struct RenderQueue
	{
		std::vector<RenderItem> opaque_buckets;
		std::vector<RenderItem> transparent_buckets;
		std::vector<RenderItem> shadow_casters;

		void clear() noexcept
		{
			opaque_buckets.clear();
			transparent_buckets.clear();
			shadow_casters.clear();
		}
	};

}