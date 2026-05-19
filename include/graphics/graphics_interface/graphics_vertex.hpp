#pragma once
#include "common/common_graphics.hpp"
#include <directx/d3d12.h>

#include "core/core_interface/core_vertex.hpp"

namespace slabb::graphics
{
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