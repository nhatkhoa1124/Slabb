#pragma once
#include "common/common_graphics.hpp"

#include "graphics/render_graph.hpp"
#include "graphics/context.hpp"

namespace slabb::graphics
{
	/**
	* @brief Handles render graphs' pipeline structuring
	*/
	class SLABB_EXPORT RenderPipeline
	{
	public:
		RenderPipeline() = default;

	/**
	* @brief Setup pipeline topology for the whole app
	*/
	void setup_pipeline(RenderGraph& render_graph,
						const std::vector<FrameContext>& frames,
						const std::vector<TextureResource*>& frame_textures,
						ID3D12RootSignature* root_signature,
						ID3D12PipelineState* pso);

private:
	void setup_main_pass(RenderGraph& render_graph,
						const std::vector<FrameContext>& frames,
						ID3D12RootSignature* root_signature,
						ID3D12PipelineState* pso);
	// TODO: Add more pass in the future

	};
}