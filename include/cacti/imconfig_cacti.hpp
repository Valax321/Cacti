#pragma once

#include "cacti/config.hpp"

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#define IMGUI_DISABLE_OBSOLETE_KEYIO
#if defined(CACTI_ENABLE_FREETYPE)
#define IMGUI_ENABLE_FREETYPE
#endif
#if defined(CACTI_BACKEND_METAL)
#define IMGUI_IMPL_METAL_CPP
#endif
