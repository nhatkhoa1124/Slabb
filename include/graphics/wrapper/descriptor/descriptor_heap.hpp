#pragma once
#include "common/common_graphics.hpp"
#include <directx/d3d12.h>

#include "../swapchain.hpp"

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

		/**
		* @brief Creates a new descriptor heap
		* @param heap_type Use enum HeapType to specify which type of heap to create
		* @param device The pointer to device that stores the heap
		* @param num_descriptors The number of desriptors in a heap to create
		*/
		void create_heap(HeapType heap_type, ID3D12Device* device, int num_descriptors);

		/*
		* @brief Create a render target view 
		* @param device The pointer to the device that stores the render target
		* @param swapchain The Swapchain object that owns the render target
		*/
		void create_render_target_view(ID3D12Device* device, const Swapchain& swapchain);

		void create_depth_stencil_view(ID3D12Device* device);
		void create_resource_descriptors(ID3D12Device* device);
		D3D12_CPU_DESCRIPTOR_HANDLE get_rtv_heap_start();
		D3D12_CPU_DESCRIPTOR_HANDLE get_dsv_heap_start();
		D3D12_GPU_DESCRIPTOR_HANDLE get_resource_heap_start();

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