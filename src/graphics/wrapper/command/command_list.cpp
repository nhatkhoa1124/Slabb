#include "graphics/wrapper/command/command_list.hpp"

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

	void CommandList::clear_screen(const D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle, const float color[4]) const
	{
		m_cmd_list->ClearRenderTargetView(rtv_handle, color, 0, nullptr);
	}

	void CommandList::set_root_signature(ID3D12RootSignature* root_signature) const
	{
		NULL_CHECK(root_signature);
		m_cmd_list->SetGraphicsRootSignature(root_signature);
	}

	void  CommandList::set_viewport(UINT num_viewports, D3D12_VIEWPORT* viewport) const
	{
		NULL_CHECK(viewport);
		m_cmd_list->RSSetViewports(num_viewports, viewport);
	}

	void  CommandList::set_scissor_rect(UINT num_rects, D3D12_RECT* rect) const
	{
		NULL_CHECK(rect);
		m_cmd_list->RSSetScissorRects(num_rects, rect);
	}

	void  CommandList::set_resource_barrier(UINT num_barriers, D3D12_RESOURCE_BARRIER* barrier) const
	{
		NULL_CHECK(barrier);
		m_cmd_list->ResourceBarrier(num_barriers, barrier);
	}

	void  CommandList::set_render_target(UINT num_render_targets, D3D12_CPU_DESCRIPTOR_HANDLE* rtv_handle,
										 D3D12_CPU_DESCRIPTOR_HANDLE* dsv_handle) const
	{
		NULL_CHECK(rtv_handle);
		NULL_CHECK(dsv_handle);
		m_cmd_list->OMSetRenderTargets(num_render_targets, rtv_handle, false, dsv_handle);
	}
}