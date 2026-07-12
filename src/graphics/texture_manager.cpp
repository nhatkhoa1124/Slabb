#include "graphics/texture_manager.hpp"

#include <directx/d3dx12.h>
#include <cstring>
#include <stdexcept>

#include "graphics/wrapper/device.hpp"
#include "graphics/wrapper/command/command_list.hpp"

namespace slabb::graphics
{
	TextureManager::TextureManager(wrapper::Device& device, RenderGraph& graph, UINT frame_count)
		: m_device{ device }
		, m_graph{ graph }
		, m_frame_count{ frame_count }
		, m_pending(frame_count)
		, m_native_device{ device.device() }
	{
	}

	TextureManager::~TextureManager() = default;

	TextureResource* TextureManager::create_rgba8(const std::string& debug_name, UINT width, UINT height,
												  const void* pixel_data)
	{
		assert(m_device.device());
		assert(pixel_data && width > 0 && height > 0);

		ID3D12Device* device = m_device.device();

		// 1. Describe and allocate the final GPU VRAM texture destination via D3D12MA.
		D3D12_RESOURCE_DESC tex_desc = {};
		tex_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		tex_desc.Width = width;
		tex_desc.Height = height;
		tex_desc.DepthOrArraySize = 1;
		tex_desc.MipLevels = 1;
		tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

		D3D12MA::ALLOCATION_DESC alloc_desc = { .HeapType = D3D12_HEAP_TYPE_DEFAULT };
		ComPtr<D3D12MA::Allocation> allocation;
		ComPtr<ID3D12Resource> texture_res;

		HRESULT hr = m_device.allocator()->CreateResource(
			&alloc_desc, &tex_desc, D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr, &allocation, IID_PPV_ARGS(&texture_res)
		);
		if (FAILED(hr))
			throw std::runtime_error("TextureManager: failed to allocate D3D12MA texture allocation.");

		// 2. Query hardware footprints for the staging buffer's aligned layout.
		UINT64 required_size = 0;
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
		device->GetCopyableFootprints(&tex_desc, 0, 1, 0, &footprint, nullptr, nullptr, &required_size);

		// 3. Allocate the upload-heap staging buffer.
		D3D12_RESOURCE_DESC buf_desc = {};
		buf_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		buf_desc.Width = required_size;
		buf_desc.Height = 1;
		buf_desc.DepthOrArraySize = 1;
		buf_desc.MipLevels = 1;
		buf_desc.SampleDesc.Count = 1;
		buf_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		D3D12MA::ALLOCATION_DESC upload_alloc_desc = { .HeapType = D3D12_HEAP_TYPE_UPLOAD };

		// Each frame's pending uploads live in a vector so multiple texture create calls in one
		// frame all get flushed before render passes touch them.
		std::vector<PendingCopy>& slot = ring_for(m_current_frame);
		slot.push_back({});
		PendingCopy& entry = slot.back();

		hr = m_device.allocator()->CreateResource(
			&upload_alloc_desc, &buf_desc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, &entry.src_alloc, IID_PPV_ARGS(&entry.src)
		);
		if (FAILED(hr))
			throw std::runtime_error("TextureManager: failed to allocate upload-heap staging buffer.");

		// 4. Map and copy row-by-row to honour 256-byte row pitch alignment.
		std::byte* mapped = nullptr;
		entry.src->Map(0, nullptr, reinterpret_cast<void**>(&mapped));
		const std::byte* src_pixels = reinterpret_cast<const std::byte*>(pixel_data);
		for (UINT y = 0; y < height; ++y)
		{
			std::memcpy(
				mapped + footprint.Offset + (y * footprint.Footprint.RowPitch),
				src_pixels + (y * width * 4),
				width * 4
			);
		}
		entry.src->Unmap(0, nullptr);
		entry.dst = texture_res.Get();
		entry.dst_alloc = std::move(allocation);
		entry.footprint = footprint;

		// 5. Register with the graph. The TextureResource takes a non-owning view; the underlying
		//    ID3D12Resource is kept alive by the D3D12MA allocation the manager owns. We must seed
		//    its tracked state at COPY_DEST so the render graph's barrier pass agrees with the
		//    transition flush_uploads will record below (otherwise the graph would emit a redundant
		//    transition from COMMON the GPU never actually sees).
		TextureResource* graph_resource = m_graph.create_resource<TextureResource>(
			debug_name, TextureUsage::COMMON);
		graph_resource->set_native_resource(texture_res.Get());
		graph_resource->set_state(D3D12_RESOURCE_STATE_COPY_DEST);

		return graph_resource;
	}

	TextureResource* TextureManager::create_depth(const std::string& debug_name, UINT width, UINT height)
	{
		ID3D12Device* device = m_device.device();
		assert(device);

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = 0;
		desc.Width = width;
		desc.Height = height;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clear_value = {};
		clear_value.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		clear_value.DepthStencil.Depth = 1.0f;
		clear_value.DepthStencil.Stencil = 0;

		CD3DX12_HEAP_PROPERTIES heap_props(D3D12_HEAP_TYPE_DEFAULT);

		ComPtr<ID3D12Resource> resource;
		HRESULT hr = device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clear_value,
			IID_PPV_ARGS(&resource)
		);
		if (FAILED(hr))
			throw std::runtime_error("TextureManager: failed to allocate committed depth-stencil buffer.");

		TextureResource* graph_resource = m_graph.create_resource<TextureResource>(
			debug_name, TextureUsage::DEPTH_STENCIL_BUFFER);
		graph_resource->set_native_resource(resource.Get());
		graph_resource->set_state(D3D12_RESOURCE_STATE_DEPTH_WRITE);

		return graph_resource;
	}

	TextureResource* TextureManager::import_external(const std::string& debug_name,
													 ID3D12Resource* native_resource,
													 TextureUsage usage)
	{
		assert(native_resource);

		TextureResource* graph_resource = m_graph.create_resource<TextureResource>(debug_name, usage);
		graph_resource->set_native_resource(native_resource);
		return graph_resource;
	}

	void TextureManager::set_current_frame(UINT frame_index)
	{
		// Drop any pending uploads recorded against the previous frame in this ring slot. By the
		// time we get here the renderer has already flushed its fence for that frame, so the
		// GPU is done reading those upload buffers.
		std::vector<PendingCopy>& slot = ring_for(frame_index);
		slot.clear();
		m_current_frame = frame_index;
	}

	void TextureManager::flush_uploads(wrapper::command::CommandList& cmd_list)
	{
		ID3D12GraphicsCommandList* native = cmd_list.command_list();
		assert(native);

		std::vector<PendingCopy>& slot = ring_for(m_current_frame);
		if (slot.empty())
		{
			return;
		}

		// CopyTextureRegion requires the destination to be in COPY_DEST. The state transition
		// COPY_DEST -> PIXEL_SHADER_RESOURCE is handled by the render graph (via reads_from())
		// rather than here, so we don't double-transition on the same resource.
		for (const PendingCopy& entry : slot)
		{
			if (!entry.dst || !entry.src) continue;

			CD3DX12_TEXTURE_COPY_LOCATION dst_loc(entry.dst, 0);
			CD3DX12_TEXTURE_COPY_LOCATION src_loc(entry.src.Get(), entry.footprint);
			native->CopyTextureRegion(&dst_loc, 0, 0, 0, &src_loc, nullptr);
		}
	}
}