#pragma once
#include "common/common.hpp"
#include <vector>

#include "../core_interface/core_vertex.hpp"

using slabb::core::Vertex;

namespace slab::core::model
{
	class SLABB_EXPORT Mesh
	{
	public:
		Mesh();
	private:
		std::vector<Vertex> m_vertices;
	};
}