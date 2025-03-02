#pragma once

// First include the SDL headers to get their actual definitions
#include <SDL2/SDL.h>
#include <SDL2/SDL_blendmode.h>

// This header provides workarounds for SDL-related issues and IntelliSense problems

// Create a namespace for SDL-related helpers
namespace pksm {
namespace sdl {

// Create type-safe wrappers for SDL blend modes that IntelliSense can recognize
// These will use the actual SDL values at runtime, not hardcoded values

// Define our own type to avoid IntelliSense errors
#ifdef __INTELLISENSE__
// Only visible to IntelliSense, not the actual compiler
typedef int SDL_BlendMode_Fix;
#define SDL_BLENDMODE_NONE_FIX 0x00000000
#define SDL_BLENDMODE_BLEND_FIX 0x00000001
#define SDL_BLENDMODE_ADD_FIX 0x00000002
#define SDL_BLENDMODE_MOD_FIX 0x00000004
#define SDL_BLENDMODE_MUL_FIX 0x00000008
#else
// For the actual compiler, just alias the real types
typedef SDL_BlendMode SDL_BlendMode_Fix;
#define SDL_BLENDMODE_NONE_FIX SDL_BLENDMODE_NONE
#define SDL_BLENDMODE_BLEND_FIX SDL_BLENDMODE_BLEND
#define SDL_BLENDMODE_ADD_FIX SDL_BLENDMODE_ADD
#define SDL_BLENDMODE_MOD_FIX SDL_BLENDMODE_MOD
#define SDL_BLENDMODE_MUL_FIX SDL_BLENDMODE_MUL
#endif

/**
 * @brief Get the SDL_BLENDMODE_NONE value
 * @return The SDL blend mode for no blending (dstRGBA = srcRGBA)
 */
inline SDL_BlendMode_Fix BlendModeNone() {
    return SDL_BLENDMODE_NONE_FIX;
}

/**
 * @brief Get the SDL_BLENDMODE_BLEND value
 * @return The SDL blend mode for alpha blending
 *         (dstRGB = (srcRGB * srcA) + (dstRGB * (1-srcA)), dstA = srcA + (dstA * (1-srcA)))
 */
inline SDL_BlendMode_Fix BlendModeBlend() {
    return SDL_BLENDMODE_BLEND_FIX;
}

/**
 * @brief Get the SDL_BLENDMODE_ADD value
 * @return The SDL blend mode for additive blending
 *         (dstRGB = (srcRGB * srcA) + dstRGB, dstA = dstA)
 */
inline SDL_BlendMode_Fix BlendModeAdd() {
    return SDL_BLENDMODE_ADD_FIX;
}

/**
 * @brief Get the SDL_BLENDMODE_MOD value
 * @return The SDL blend mode for color modulation
 *         (dstRGB = srcRGB * dstRGB, dstA = dstA)
 */
inline SDL_BlendMode_Fix BlendModeMod() {
    return SDL_BLENDMODE_MOD_FIX;
}

/**
 * @brief Get the SDL_BLENDMODE_MUL value
 * @return The SDL blend mode for color multiplication
 *         (dstRGB = (srcRGB * dstRGB) + (dstRGB * (1-srcA)), dstA = dstA)
 */
inline SDL_BlendMode_Fix BlendModeMul() {
    return SDL_BLENDMODE_MUL_FIX;
}

// You can add more SDL helper functions here as needed

}  // namespace sdl
}  // namespace pksm

// For backward compatibility with existing code, provide macros that use our functions
// These will be properly recognized by IntelliSense while still using the actual SDL values
#ifndef PKSM_SDL_COMPAT_MACROS
#define PKSM_SDL_COMPAT_MACROS
#define PKSM_SDL_BLENDMODE_NONE pksm::sdl::BlendModeNone()
#define PKSM_SDL_BLENDMODE_BLEND pksm::sdl::BlendModeBlend()
#define PKSM_SDL_BLENDMODE_ADD pksm::sdl::BlendModeAdd()
#define PKSM_SDL_BLENDMODE_MOD pksm::sdl::BlendModeMod()
#define PKSM_SDL_BLENDMODE_MUL pksm::sdl::BlendModeMul()
#endif