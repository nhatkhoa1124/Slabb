#include "common.hpp"

#define NOMINMAX
#include <wrl/client.h>

#include <cstdint>
#include <limits>

namespace slabb::graphics
{
	inline constexpr size_t SLABB_INVALID_SIZE_T = (std::numeric_limits<size_t>::max)();
}