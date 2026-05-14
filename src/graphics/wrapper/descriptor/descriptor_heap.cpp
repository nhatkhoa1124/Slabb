#include "graphics/wrapper/descriptor/descriptor_heap.hpp"

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::wrapper::descriptor
{
	DescriptorHeap::DescriptorHeap(): m_rtv_descriptor_size {0}, m_dsv_descriptor_size {0}, 
        m_resource_descriptor_size {0}
	{}

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
}