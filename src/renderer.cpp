#include "renderer.h"

namespace natsukashii::renderer
{
Renderer::Renderer(SDL_Window* window, bool maintain_aspect_ratio) : maintain_aspect_ratio(maintain_aspect_ratio)
{
  int w, h;
  SDL_GetWindowSize(window, &w, &h);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  ImGui::CreateContext();
  ImGuiSDL::Initialize(renderer, w, h);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, GB_WIDTH, GB_HEIGHT);
}

void Renderer::DrawFrame(byte* buffer)
{
  SDL_UpdateTexture(texture, nullptr, buffer, 4 * GB_WIDTH);
}

} // natsukashii::renderer