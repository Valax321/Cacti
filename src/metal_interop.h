#pragma once

#include <SDL2/SDL.h>

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

CA::MetalLayer* GetMetalLayerFromSDL(SDL_MetalView view);
