#include "graphics/wrapper/resource/resource_heap.hpp"
#include <directx/d3dx12.h>

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::wrapper::resource
{
	BufferHeap::BufferHeap(HeapType type) :
		m_allocated_size{0}
	{
		switch (type)
		{
		case HeapType::UPLOAD:
			m_heap_type = D3D12_HEAP_TYPE_UPLOAD;
			break;
		case HeapType::DEFAULT:
			m_heap_type = D3D12_HEAP_TYPE_DEFAULT;
			break;
		default:
			m_heap_type = D3D12_HEAP_TYPE_DEFAULT;
			break;
		}
	}

	void BufferHeap::create_heap(ID3D12Device* device, const size_t buffer_size)
	{
		assert(m_buffer == nullptr); // Make sure each heap can only be created once
		assert(buffer_size > 0);
		NULL_CHECK(device);
		CD3DX12_HEAP_PROPERTIES heap_prop(m_heap_type);
		const auto desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);
		spdlog::info("Creating buffer heap...");
		SLABB_CHECK(device->CreateCommittedResource(
			&heap_prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			(m_heap_type == D3D12_HEAP_TYPE_UPLOAD) ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_buffer)
		));
		m_allocated_size = buffer_size;
		spdlog::info("Buffer heap created successfully");
	}

	void BufferHeap::upload_data(std::span<const std::byte> data)
	{
		assert(data.size_bytes() <= m_allocated_size);
		switch (m_heap_type)
		{
		case D3D12_HEAP_TYPE_UPLOAD:
			spdlog::info("Copying data to UPLOAD heap...");
			UINT8* p_vertex_data_begin;
			CD3DX12_RANGE read_range(0, 0);
			SLABB_CHECK(m_buffer->Map(0, &read_range, reinterpret_cast<void**>(&p_vertex_data_begin)));
			std::memcpy(p_vertex_data_begin, data.data(), data.size_bytes());
			m_buffer->Unmap(0, nullptr);
			spdlog::info("Data copied successfully");
			break;
		case D3D12_HEAP_TYPE_DEFAULT:
			spdlog::info("DEFAULT type not supported for now");
			break;
		default:
			spdlog::warn("Data copy failed: Invalid heap type");
			break;
		}
	}
}