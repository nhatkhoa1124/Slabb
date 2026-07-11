#pragma once
#include <DirectXMath.h>

namespace slabb::graphics
{
	struct TransformCB
	{
		DirectX::XMMATRIX mvp_matrix;
	};
}