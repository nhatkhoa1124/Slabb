#include "graphics/wrapper/pipeline/graphics_pipeline.hpp"
#include <directx/d3dx12.h>

#include "graphics/tools/debug.hpp"
#include "graphics/wrapper/shader_manager.hpp"
#include "graphics/graphics_interface/graphics_vertex.hpp"

namespace slabb::graphics::wrapper::pipeline
{
	GraphicsPipeline::GraphicsPipeline()
	{

	}

	void GraphicsPipeline::create_graphics_pipeline(ID3D12Device* device, ID3D12RootSignature* root_signature,
													std::vector<D3D12_INPUT_ELEMENT_DESC> input_elements)
	{
		NULL_CHECK(device);
		NULL_CHECK(root_signature);
		assert(m_vertex_shader != nullptr);
		assert(m_pixel_shader != nullptr);

		m_root_signature = root_signature;

		spdlog::info("Creating graphics pipeline...");

		D3D12_SHADER_BYTECODE vs_bytecode = {};
		vs_bytecode.pShaderBytecode = m_vertex_shader->GetBufferPointer();
		vs_bytecode.BytecodeLength = m_vertex_shader->GetBufferSize();
		D3D12_SHADER_BYTECODE ps_bytecode = {};
		ps_bytecode.pShaderBytecode = m_pixel_shader->GetBufferPointer();
		ps_bytecode.BytecodeLength = m_pixel_shader->GetBufferSize();
		D3D12_INPUT_LAYOUT_DESC input_layout =
		{
			.pInputElementDescs = input_elements.data(),
			.NumElements = static_cast<UINT>(input_elements.size())
		};

		GraphicsPipelineBuilder builder = {};
		builder.with_root_signature(root_signature);
		builder.with_input_layout(input_layout);
		builder.with_vertex_shader(vs_bytecode);
		builder.with_pixel_shader(ps_bytecode);
		builder.with_rasterizer_state(CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT));
		builder.with_blend_state(CD3DX12_BLEND_DESC(D3D12_DEFAULT));
		builder.with_depth_stencil_state(
			D3D12_DEPTH_STENCIL_DESC 
			{
				.DepthEnable = false,
				.StencilEnable = false
			}
		);
		builder.with_sample_mask(UINT_MAX);
		builder.with_sample_desc(
			DXGI_SAMPLE_DESC
			{
				.Count = 1 // No AA, take 1 sample only
			}
		);
		builder.with_primitive_topology_type(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		builder.with_num_render_targets(1);
		builder.with_rtv_format(0, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_pso = builder.build_pipeline(device);
		spdlog::info("Graphics pipeline created successfully");
	}

	void GraphicsPipeline::load_shaders(const std::string& vertex_path, const std::string& pixel_path)
	{
		const std::wstring w_vertex_path = std::wstring(vertex_path.begin(), vertex_path.end());
		const std::wstring w_pixel_path = std::wstring(pixel_path.begin(), pixel_path.end());
		m_vertex_shader = ShaderManager::compile_shader(w_vertex_path, "VSMain", "vs_5_0");
		m_pixel_shader = ShaderManager::compile_shader(w_pixel_path, "PSMain", "ps_5_0");
	}
}

namespace slabb::graphics::wrapper::pipeline
{
	// AI-written
	GraphicsPipelineBuilder::GraphicsPipelineBuilder()
	{
		m_pso_desc = {};

		// 2. Establish safe hardware defaults so the builder doesn't crash 
		//    if you accidentally omit optional configurations.
		m_pso_desc.SampleMask = UINT_MAX; // Use all MSAA sample points by default

		// Setup a standard default rasterizer state (solid rendering, backface culling)
		m_pso_desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		m_pso_desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		m_pso_desc.RasterizerState.FrontCounterClockwise = FALSE;
		m_pso_desc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		m_pso_desc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		m_pso_desc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		m_pso_desc.RasterizerState.DepthClipEnable = TRUE;
		m_pso_desc.RasterizerState.MultisampleEnable = FALSE;
		m_pso_desc.RasterizerState.AntialiasedLineEnable = FALSE;
		m_pso_desc.RasterizerState.ForcedSampleCount = 0;
		m_pso_desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		// Setup standard default blend state (Opaque rendering, no alpha mixing)
		m_pso_desc.BlendState.AlphaToCoverageEnable = FALSE;
		m_pso_desc.BlendState.IndependentBlendEnable = FALSE;
		const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
		{
			FALSE, FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL,
		};
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			m_pso_desc.BlendState.RenderTarget[i] = defaultRenderTargetBlendDesc;
		}

		// Setup standard 1x MSAA default sample descriptor
		m_pso_desc.SampleDesc.Count = 1;
		m_pso_desc.SampleDesc.Quality = 0;
	}

	void GraphicsPipelineBuilder::with_root_signature(ID3D12RootSignature* root_signature)
	{
		m_pso_desc.pRootSignature = root_signature;
	}

	void GraphicsPipelineBuilder::with_vertex_shader(D3D12_SHADER_BYTECODE vertex_shader)
	{
		m_pso_desc.VS = vertex_shader;
	}

	void GraphicsPipelineBuilder::with_pixel_shader(D3D12_SHADER_BYTECODE pixel_shader)
	{
		m_pso_desc.PS = pixel_shader;
	}

	void GraphicsPipelineBuilder::with_blend_state(D3D12_BLEND_DESC blend_state)
	{
		m_pso_desc.BlendState = blend_state;
	}

	void GraphicsPipelineBuilder::with_sample_mask(UINT mask)
	{
		m_pso_desc.SampleMask = mask;
	}

	void GraphicsPipelineBuilder::with_rasterizer_state(D3D12_RASTERIZER_DESC rasterizer_state)
	{
		m_pso_desc.RasterizerState = rasterizer_state;
	}

	void GraphicsPipelineBuilder::with_depth_stencil_state(D3D12_DEPTH_STENCIL_DESC depth_stencil_state)
	{
		m_pso_desc.DepthStencilState = depth_stencil_state;
	}

	void GraphicsPipelineBuilder::with_input_layout(D3D12_INPUT_LAYOUT_DESC input_layout)
	{
		m_pso_desc.InputLayout = input_layout;
	}

	void GraphicsPipelineBuilder::with_primitive_topology_type(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology_type)
	{
		m_pso_desc.PrimitiveTopologyType = topology_type;
	}

	void GraphicsPipelineBuilder::with_num_render_targets(UINT count)
	{
		m_pso_desc.NumRenderTargets = count;
	}

	void GraphicsPipelineBuilder::with_rtv_format(UINT index, DXGI_FORMAT format)
	{
		assert(index < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
		m_pso_desc.RTVFormats[index] = format;
	}

	void GraphicsPipelineBuilder::with_sample_desc(DXGI_SAMPLE_DESC sample_desc)
	{
		m_pso_desc.SampleDesc = sample_desc;
	}

	ComPtr<ID3D12PipelineState> GraphicsPipelineBuilder::build_pipeline(ID3D12Device* device)
	{
		NULL_CHECK(device);
		ComPtr<ID3D12PipelineState> pso;
		SLABB_CHECK(device->CreateGraphicsPipelineState(&m_pso_desc,IID_PPV_ARGS(&pso)));
		return pso;
	}
}