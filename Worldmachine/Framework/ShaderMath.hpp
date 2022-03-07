#pragma once

#include "../Core/Base.hpp"

#if WORLDMACHINE(CPP)

#include <mtl/mtl.hpp>
namespace metal = mtl;

#else // WORLDMACHINE(CPP)

#include <metal_stdlib>

#endif // WORLDMACHINE(CPP)

