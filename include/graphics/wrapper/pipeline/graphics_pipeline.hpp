#pragma once
#include "common/common_graphics.hpp"
#include <directx/d3d12.h>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace slabb::graphics::wrapper::pipeline
{
	/**
	* @brief The wrapper class for creating the graphics pipeline
	*/
	class SLABB_EXPORT GraphicsPipeline
	{
	public:
		GraphicsPipeline();
		void create_graphics_pipeline(ID3D12Device* device, ID3D12RootSignature* root_signature, 
									  std::vector<D3D12_INPUT_ELEMENT_DESC> input_elements);
		void load_shaders(const std::string& vertex_path, const std::string& pixel_path);

		[[nodiscard]] inline ID3D12PipelineState* pipeline_state_object() const { return m_pso.Get(); }
		[[nodiscard]] inline ID3D12RootSignature* root_signature() const { return m_root_signature.Get(); }
	private:

	private:
		ComPtr<ID3D12PipelineState> m_pso;
		ComPtr<ID3DBlob> m_vertex_shader;
		ComPtr<ID3DBlob> m_pixel_shader;
		ComPtr<ID3D12RootSignature> m_root_signature;
	};

	/**
	* @brief Builder class for graphics pipeline
	*/
	class SLABB_EXPORT GraphicsPipelineBuilder
	{
	public:
		GraphicsPipelineBuilder();
		void with_root_signature(ID3D12RootSignature* root_signature);
		void with_vertex_shader(D3D12_SHADER_BYTECODE vertex_shader);
		void with_pixel_shader(D3D12_SHADER_BYTECODE pixel_shader);
		void with_blend_state(D3D12_BLEND_DESC blend_state);
		void with_sample_mask(UINT mask);
		void with_rasterizer_state(D3D12_RASTERIZER_DESC rasterizer_state);
		void with_depth_stencil_state(D3D12_DEPTH_STENCIL_DESC depth_stencil_state);
		void with_input_layout(D3D12_INPUT_LAYOUT_DESC input_layout);
		void with_primitive_topology_type(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology_type);
		void with_num_render_targets(UINT count);
		void with_rtv_format(UINT index, DXGI_FORMAT format);
		void with_sample_desc(DXGI_SAMPLE_DESC sample_desc);
		ComPtr<ID3D12PipelineState> build_pipeline(ID3D12Device* device);
	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_pso_desc;
	};
}