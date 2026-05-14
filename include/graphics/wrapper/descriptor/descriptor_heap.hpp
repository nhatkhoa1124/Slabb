#pragma once
#include "common/common_graphics.hpp"
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

enum class HeapType
{
	RENDER_TARGET,	// RTV Heap
	DEPTH,			// DSV Heap
	RESOURCE		// CBV_UAV_SRV Heap
};

namespace slabb::graphics::wrapper::descriptor
{
	class DescriptorHeap
	{
	public:
		DescriptorHeap();
		void create_heap(HeapType heap_type, ID3D12Device* device, int num_descriptors);

		[[nodiscard]] inline ID3D12DescriptorHeap* rtv_heap() const { return m_rtv_heap.Get(); }
		[[nodiscard]] inline ID3D12DescriptorHeap* dsv_heap() const { return m_dsv_heap.Get(); }
		[[nodiscard]] inline ID3D12DescriptorHeap* resource_heap() const { return m_resource_heap.Get(); }
		[[nodiscard]] inline UINT rtv_heap_size() const { return m_rtv_descriptor_size; }
		[[nodiscard]] inline UINT dsv_heap_size() const { return m_dsv_descriptor_size; }
		[[nodiscard]] inline UINT resource_heap_size() const { return m_resource_descriptor_size; }

	private:
		ComPtr<ID3D12DescriptorHeap> m_rtv_heap;
		ComPtr<ID3D12DescriptorHeap> m_dsv_heap;
		ComPtr<ID3D12DescriptorHeap> m_resource_heap;

		UINT m_rtv_descriptor_size;
		UINT m_dsv_descriptor_size;
		UINT m_resource_descriptor_size;
	};
}