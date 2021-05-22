#pragma once
#include "common.h"
#include "imgui.h"
#include "imgui_sdl.h"
#include <SDL2/SDL.h>

namespace natsukashii::renderer
{
constexpr int GB_WIDTH = 160, GB_HEIGHT = 144;

struct Renderer
{
  ~Renderer() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
  }
  Renderer(SDL_Window* window, bool maintain_aspect_ratio = true);
  bool maintain_aspect_ratio = true;
  void DrawFrame(byte* buffer);
  SDL_Renderer* renderer = nullptr;
  SDL_Texture* texture = nullptr;
};
} // natsukashii::renderer