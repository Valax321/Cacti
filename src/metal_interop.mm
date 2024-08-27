#include "cacti/config.hpp"
#if defined(CACTI_BACKEND_METAL)
#include "metal_interop.h"

CA::MetalLayer* GetMetalLayerFromSDL(SDL_MetalView view) {
    CA::MetalLayer* cppLayer = (__bridge CA::MetalLayer*)(CAMetalLayer*)view;
    return cppLayer;
}
#endif