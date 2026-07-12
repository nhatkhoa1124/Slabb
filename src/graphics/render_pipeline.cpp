#include "graphics/render_pipeline.hpp"
#include <directx/d3dx12.h>

#include "graphics/wrapper/command/command_list.hpp"
#include "graphics/render_queue.hpp"

namespace slabb::graphics
{
	void RenderPipeline::setup_pipeline(RenderGraph& render_graph,
										const std::vector<FrameContext>& frames,
										ID3D12RootSignature* root_signature,
										ID3D12PipelineState* pso)
	{
		setup_main_pass(render_graph, frames, root_signature, pso);
	}

	void RenderPipeline::setup_main_pass(RenderGraph& render_graph,
										const std::vector<FrameContext>& frames,
										ID3D12RootSignature* root_signature,
										ID3D12PipelineState* pso)
	{
		auto& main_pass = render_graph.add_pass("Main");
		main_pass.set_root_signature(root_signature);
		main_pass.set_pipeline_state(pso);

		for (const auto& frame : frames)
		{
			main_pass.reads_from(frame.camera_constant_buffer);
		}

		main_pass.record([&frames](wrapper::command::CommandList & cmd, UINT frame_idx, const RenderQueue& queue)
			{
				const auto& current_frame = frames[frame_idx];

				D3D12_GPU_VIRTUAL_ADDRESS cbv_address = current_frame.camera_constant_buffer->constant_view().BufferLocation;
				cmd.set_graphics_root_cbv(0, cbv_address);

				// Traverse opaque bucket
				for (const auto& item : queue.opaque_buckets)
				{
					// TODO: Bind the unique per-object transform matrix to Slot 1 in the root signature
					// e.g., cmd.set_graphics_root_cbv(1, item.constant_buffer_address);

					// Bind vertex buffer layout state
					cmd.set_vertex_buffers(0, 1, &item.vertex_buffer->vertex_view());

					// Dynamic indexed fallback determination
					if (item.index_buffer)
					{
						cmd.command_list()->IASetIndexBuffer(&item.index_buffer->index_view());
						cmd.command_list()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
						cmd.command_list()->DrawIndexedInstanced(item.index_count, 1, 0, 0, 0);
					}
					else
					{
						cmd.command_list()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
						cmd.draw_instanced(item.vertex_count, 1, 0, 0);
					}
				}
			});
	}
}