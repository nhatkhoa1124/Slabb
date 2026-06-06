#include "graphics/wrapper/command/command_list.hpp"
#include <directx/d3dx12.h>

#include "graphics/tools/debug.hpp"


namespace slabb::graphics::wrapper::command
{
	CommandList::CommandList()
	{
	}

	void CommandList::create_command_list(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type,
										  ID3D12CommandAllocator* allocator, ID3D12PipelineState* pipeline_state)
	{
		NULL_CHECK(device);
		NULL_CHECK(allocator);
		spdlog::info("Creating command list...");
		SLABB_CHECK(device->CreateCommandList(0, type, allocator, pipeline_state, IID_PPV_ARGS(&m_cmd_list)));
		spdlog::info("Command list created successfully");
	}

	template <typename T>
	void CommandList::record_command(ID3D12CommandAllocator* allocator, ID3D12Resource* render_target,
									 ID3D12RootSignature* root_signature, const D3D12_CPU_DESCRIPTOR_HANDLE* rtv_handle,
									 const D3D12_VIEWPORT* viewport, const D3D12_RECT* rect,
									 ID3D12PipelineState* pipeline_state, T&& callback)
	{
		// TO-DO: Refactor this into a render graph pass
		allocator->Reset();
		reset(allocator, pipeline_state);
		set_root_signature(root_signature);
		set_viewport(1, viewport);
		set_scissor_rect(1, rect);

		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_target,
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		set_resource_barrier(1, &barrier);
		set_render_target(1, rtv_handle, nullptr);
		const float clear_color[4] = { 0.1f, 0.2f, 0.3f, 1.0f };
		clear_screen(*rtv_handle, clear_color);

		// Start recording
		if constexpr (std::is_invocable_v<T, ID3D12GraphicsCommandList*>)
		{
			if (callback)
			{
				callback(m_cmd_list.Get());
			}
		}
		// Recording end 

		barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_target,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);
		set_resource_barrier(1, &barrier);
		close();
	}

	void CommandList::reset(ID3D12CommandAllocator* allocator, ID3D12PipelineState* pipeline_state)
	{
		NULL_CHECK(allocator);
		NULL_CHECK(pipeline_state);
		NULL_CHECK(m_cmd_list);

		SLABB_CHECK(m_cmd_list->Reset(allocator, pipeline_state));
	}

	void CommandList::close()
	{
		NULL_CHECK(m_cmd_list);

		SLABB_CHECK(m_cmd_list->Close());
	}

	void CommandList::clear_screen(const D3D12_CPU_DESCRIPTOR_HANDLE& rtv_handle, const float color[4]) const
	{
		m_cmd_list->ClearRenderTargetView(rtv_handle, color, 0, nullptr);
	}

	void CommandList::set_root_signature(ID3D12RootSignature* root_signature) const
	{
		NULL_CHECK(root_signature);
		m_cmd_list->SetGraphicsRootSignature(root_signature);
	}

	void  CommandList::set_viewport(UINT num_viewports, const D3D12_VIEWPORT* viewport) const
	{
		NULL_CHECK(viewport);
		m_cmd_list->RSSetViewports(num_viewports, viewport);
	}

	void  CommandList::set_scissor_rect(UINT num_rects, const D3D12_RECT* rect) const
	{
		NULL_CHECK(rect);
		m_cmd_list->RSSetScissorRects(num_rects, rect);
	}

	void  CommandList::set_resource_barrier(UINT num_barriers, const D3D12_RESOURCE_BARRIER* barrier) const
	{
		NULL_CHECK(barrier);
		m_cmd_list->ResourceBarrier(num_barriers, barrier);
	}

	void  CommandList::set_render_target(UINT num_render_targets, const D3D12_CPU_DESCRIPTOR_HANDLE* rtv_handle,
										 const D3D12_CPU_DESCRIPTOR_HANDLE* dsv_handle) const
	{
		m_cmd_list->OMSetRenderTargets(num_render_targets, rtv_handle, false, dsv_handle);
	}
}

template void slabb::graphics::wrapper::command::CommandList::record_command<std::nullptr_t>(
	ID3D12CommandAllocator* allocator,
	ID3D12Resource* render_target,
	ID3D12RootSignature* root_signature,
	const D3D12_CPU_DESCRIPTOR_HANDLE* rtv_handle,
	const D3D12_VIEWPORT* viewport,
	const D3D12_RECT* rect,
	ID3D12PipelineState* pipeline_state,
	std::nullptr_t&& callback // Note the double ampersand matching T&&
);