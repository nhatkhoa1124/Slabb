#pragma once
#include "common/common_graphics.hpp"
#include <directx/d3d12.h>
#include <span>

using Microsoft::WRL::ComPtr;

namespace slabb::graphics::wrapper::resource
{
	enum class HeapType
	{
		UPLOAD,
		DEFAULT
	};

	class SLABB_EXPORT BufferHeap
	{
	public:
		BufferHeap(HeapType type);
		void create_heap(ID3D12Device* device, const size_t buffer_size);
		void upload_data(std::span<const std::byte> data);

		[[nodiscard]] ID3D12Resource* resource_heap() const { return m_buffer.Get(); }
		[[nodiscard]] size_t allocated_size() const { return m_allocated_size; }
		[[nodiscard]] D3D12_HEAP_TYPE heap_type() const { return m_heap_type; }
	private:
		ComPtr<ID3D12Resource> m_buffer;
		D3D12_HEAP_TYPE m_heap_type;
		size_t m_allocated_size;
	};
}