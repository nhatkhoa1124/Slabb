#pragma once
#include "common/common_graphics.hpp"
#include <directx/d3d12.h>

#include "core/core_interface/core_vertex.hpp"

namespace slabb::graphics
{
	/**
	* @brief Tightly-packed RGBA8 image data shared from core to graphics.
	* Mirrors core::model::Image; duplicated here to avoid a graphics->core dependency.
	*/
	struct TextureImage
	{
		int width{ 0 };
		int height{ 0 };
		std::vector<uint8_t> pixels;
	};

	/**
	* @brief Interface for core-graphics Mesh
	*/
	struct GraphicsMesh
	{
		const void* vertex_data;
		size_t vertex_count;
		size_t vertex_stride;

		const uint32_t* index_data;
		size_t index_count;

		int base_color_texture{ -1 }; // index into GraphicsModel::images
	};

	/**
	* @brief Interface for core-graphics Model
	*/
	struct GraphicsModel
	{
		std::vector<GraphicsMesh> meshes;
		std::vector<TextureImage> images;
		DirectX::XMMATRIX transform;
	};

	class SLABB_EXPORT GraphicsVertex
	{
	public:
		/**
		* @brief This method translate core layer's vertex specification to the graphics layer
		* @param core_layout This is the layout of vertex attributes from core layer
		*/
		[[nodiscard]] inline static std::vector<D3D12_INPUT_ELEMENT_DESC> get_vertex_input_layout
		(std::vector <core::VertexAttribute> core_layout)
		{
			std::vector<D3D12_INPUT_ELEMENT_DESC> element_descs;
			UINT current_offset = 0;
			for (const auto& attribute : core_layout)
			{
				D3D12_INPUT_ELEMENT_DESC desc =
				{
					.SemanticIndex = 0,
					.InputSlot = 0,
					.AlignedByteOffset = current_offset,
					.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
					.InstanceDataStepRate = 0
				};

				switch (attribute)
				{
				case core::VertexAttribute::Position:
					desc.SemanticName = "POSITION";
					desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					current_offset += 12;
					break;
				case core::VertexAttribute::Color:
					desc.SemanticName = "COLOR";
					desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					current_offset += 16;
					break;
				case core::VertexAttribute::Normal:
					desc.SemanticName = "NORMAL";
					desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					current_offset += 12;
					break;
				case core::VertexAttribute::TexCoord:
					desc.SemanticName = "TEXCOORD";
					desc.Format = DXGI_FORMAT_R32G32_FLOAT;
					current_offset += 8;
					break;
				}
				element_descs.push_back(desc);
			}
			return element_descs;
		}
	};
}