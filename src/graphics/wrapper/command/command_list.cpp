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

	void CommandList::reset(ID3D12CommandAllocator* allocator, ID3D12PipelineState* pipeline_state)
	{
		NULL_CHECK(allocator);
		NULL_CHECK(m_cmd_list);
		SLABB_CHECK(m_cmd_list->Reset(allocator, pipeline_state));
	}

	void CommandList::close()
	{
		NULL_CHECK(m_cmd_list);
		SLABB_CHECK(m_cmd_list->Close());
	}

	void CommandList::clear_render_target(const D3D12_CPU_DESCRIPTOR_HANDLE& rtv_handle, const float color[4]) const
	{
		NULL_CHECK(m_cmd_list);
		m_cmd_list->ClearRenderTargetView(rtv_handle, color, 0, nullptr);
	}

	void CommandList::clear_depth_stencil(const D3D12_CPU_DESCRIPTOR_HANDLE& dsv_handle, float depth, UINT stencil) const
	{
		NULL_CHECK(m_cmd_list);
		m_cmd_list->ClearDepthStencilView(dsv_handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
	}

	void CommandList::set_vertex_buffers(UINT start_slot, UINT num_views, const D3D12_VERTEX_BUFFER_VIEW* views) const
	{
		NULL_CHECK(m_cmd_list);
		m_cmd_list->IASetVertexBuffers(start_slot, num_views, views);
	}

	void CommandList::set_root_signature(ID3D12RootSignature* root_signature) const
	{
		NULL_CHECK(m_cmd_list);
		NULL_CHECK(root_signature);
		m_cmd_list->SetGraphicsRootSignature(root_signature);
	}

	void CommandList::set_viewport(UINT num_viewports, const D3D12_VIEWPORT* viewport) const
	{
		NULL_CHECK(m_cmd_list);
		NULL_CHECK(viewport);
		m_cmd_list->RSSetViewports(num_viewports, viewport);
	}

	void CommandList::set_scissor_rect(UINT num_rects, const D3D12_RECT* rect) const
	{
		NULL_CHECK(m_cmd_list);
		NULL_CHECK(rect);
		m_cmd_list->RSSetScissorRects(num_rects, rect);
	}

	void CommandList::set_resource_barrier(UINT num_barriers, const D3D12_RESOURCE_BARRIER* barrier) const
	{
		NULL_CHECK(m_cmd_list);
		NULL_CHECK(barrier);
		m_cmd_list->ResourceBarrier(num_barriers, barrier);
	}

	void CommandList::set_render_target(UINT num_render_targets, const D3D12_CPU_DESCRIPTOR_HANDLE* rtv_handle,
										 const D3D12_CPU_DESCRIPTOR_HANDLE* dsv_handle) const
	{
		NULL_CHECK(m_cmd_list);
		m_cmd_list->OMSetRenderTargets(num_render_targets, rtv_handle, false, dsv_handle);
	}

	void CommandList::set_pipline_state(ID3D12PipelineState* pipeline_state) const
	{
		NULL_CHECK(m_cmd_list);
		m_cmd_list->SetPipelineState(pipeline_state);
	}

	void CommandList::draw_instanced(UINT vertex_count_per_instance, UINT instance_count,
								UINT start_vertex_location, UINT start_instance_location) const
	{
		NULL_CHECK(m_cmd_list);
		m_cmd_list->DrawInstanced(vertex_count_per_instance, instance_count, start_vertex_location, start_instance_location);
	}
}