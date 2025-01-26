#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <string>

class Screen {
protected:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool exit = false;

public:
    Screen(SDL_Window* window, SDL_Renderer* renderer);
    virtual ~Screen() = default;

    // Main screen functions
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void handleEvent(const SDL_Event& event) = 0;

    // Common utility functions
    bool shouldExit() const { return exit; }
    void setExit(bool value) { exit = value; }
    
    // Screen dimensions
    static constexpr int SCREEN_WIDTH = 1280;  // 720p in docked
    static constexpr int SCREEN_HEIGHT = 720;
}; 