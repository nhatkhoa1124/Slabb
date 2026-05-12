#pragma once
#include "common/common_graphics.hpp"

using Microsoft::WRL::ComPtr;

namespace slabb::graphics::tools {
	class SLABB_EXPORT Validation
	{
	public:
		Validation() = default;

		/**
		* @brief Enables D3D12 debug layer
		*/
		static void enable_debug_layer();

		/**
		* @brief Enables GPU-Based Validation
		* @param enable Use this to turn GBV on or off
		* @warning Call this only when debugging
		*/
		static void set_gbv(bool enable);
	};
}

