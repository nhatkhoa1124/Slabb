#pragma once
#include "common/common.hpp"
#include <DirectXMath.h>
#include <vector>

namespace slabb::core
{
	/**
	* @brief This enum specifies vertex attributes in a model
	*/
	enum class VertexAttribute
	{
		Position,
		Color,
		Normal,
		TexCoord
	};

	/**
	* @brief This struct defines an interface between the core and graphics layer's vertex layout
	*/
	struct SLABB_EXPORT Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 tex_coord;

		/**
		* @brief Returns a list of vertex attributes for graphics module to use
		*/
		static std::vector<VertexAttribute> vertex_attribute()
		{
			return {
				VertexAttribute::Position,
				VertexAttribute::Color,
				VertexAttribute::Normal,
				VertexAttribute::TexCoord
			};
		}
	};
}