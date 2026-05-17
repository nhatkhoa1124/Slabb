#include "graphics/wrapper/descriptor/descriptor_heap.hpp"
#include <directx/d3dx12.h>

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::wrapper::descriptor
{
	DescriptorHeap::DescriptorHeap(): m_rtv_descriptor_size {0}, m_dsv_descriptor_size {0}, 
        m_resource_descriptor_size {0}
	{
    }

	void DescriptorHeap::create_heap(HeapType heap_type, ID3D12Device* device, int num_descriptors)
	{
		assert(num_descriptors > 0);
		NULL_CHECK(device);

        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = num_descriptors;
        desc.NodeMask = 0;

        switch (heap_type)
        {
        case HeapType::RENDER_TARGET:
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            SLABB_CHECK(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtv_heap)));
            m_rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(desc.Type);
            break;

        case HeapType::DEPTH:
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            SLABB_CHECK(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_dsv_heap)));
            m_dsv_descriptor_size = device->GetDescriptorHandleIncrementSize(desc.Type);
            break;

        case HeapType::RESOURCE:
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            SLABB_CHECK(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_resource_heap)));
            m_resource_descriptor_size = device->GetDescriptorHandleIncrementSize(desc.Type);
            break;
        }
	}

    void DescriptorHeap::create_render_target_view(ID3D12Device* device, const Swapchain& swapchain)
    {
        NULL_CHECK(device);
        D3D12_RENDER_TARGET_VIEW_DESC desc =
        {
            .Format = swapchain.format(),
            .ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
            .Texture2D = 
            {
                .MipSlice = 0,
                .PlaneSlice = 0
            }
        };

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(get_rtv_heap_start());
        for (size_t i = 0; i < swapchain.buffer_count(); i++)
        {
            device->CreateRenderTargetView(swapchain.render_target(i), &desc, rtv_handle);
            rtv_handle.Offset(1, m_rtv_descriptor_size);
        }
    }

    void DescriptorHeap::create_depth_stencil_view(ID3D12Device* device)
    {

    }

    void DescriptorHeap::create_resource_descriptors(ID3D12Device* device)
    {

    }

    D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::get_rtv_heap_start()
    {
        return m_rtv_heap->GetCPUDescriptorHandleForHeapStart();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::get_dsv_heap_start()
    {
        return m_dsv_heap->GetCPUDescriptorHandleForHeapStart();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::get_resource_heap_start()
    {
        return m_resource_heap->GetGPUDescriptorHandleForHeapStart();
    }
}