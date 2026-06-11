#pragma once
#include "common/common_graphics.hpp"
#include <directx/d3d12.h>

using Microsoft::WRL::ComPtr;

namespace slabb::graphics::wrapper::command
{
	/**
	* @brief Wrapper class for command list operations
	*/
	class SLABB_EXPORT CommandList
	{
	public:
		CommandList();
		void create_command_list(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type, 
								 ID3D12CommandAllocator* allocator, ID3D12PipelineState* pipeline_state = nullptr);
		/**
		* @brief Uploads vertex data from CPU to GPU
		*/
		void upload_vertex_buffer();

		template <typename T>
		void record_command(ID3D12CommandAllocator* allocator, ID3D12Resource* render_target,
							ID3D12RootSignature* root_signature, const D3D12_CPU_DESCRIPTOR_HANDLE* rtv_handle,
							const D3D12_VIEWPORT* viewport, const D3D12_RECT* rect,
							ID3D12PipelineState* pipeline_state, T&& callback);

		void reset(ID3D12CommandAllocator* allocator, ID3D12PipelineState* pipeline_state);
		void close();
		void clear_screen(const D3D12_CPU_DESCRIPTOR_HANDLE& rtv_handle, const float color[4]) const;

		void set_root_signature(ID3D12RootSignature* root_signature) const;
		void set_viewport(UINT num_viewports, const D3D12_VIEWPORT* viewports) const;
		void set_scissor_rect(UINT num_rects, const D3D12_RECT* rects) const;
		void set_resource_barrier(UINT num_barriers, const D3D12_RESOURCE_BARRIER* barrier) const;
		void set_render_target(UINT num_render_targets, const D3D12_CPU_DESCRIPTOR_HANDLE* rtv_handle,
							   const D3D12_CPU_DESCRIPTOR_HANDLE* dsv_handle) const;
		void set_pipline_state(ID3D12PipelineState* pipeline_state) const
		{
			m_cmd_list->SetPipelineState(pipeline_state); 
		}

		[[nodiscard]] ID3D12GraphicsCommandList* command_list() const { return m_cmd_list.Get(); }
	private:
		ComPtr<ID3D12GraphicsCommandList> m_cmd_list;
	};
}