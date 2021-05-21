#pragma once
#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include "common.h"

namespace natsukashii::renderer
{
constexpr int WIDTH = 160, HEIGHT = 144;
struct Renderer
{
  Renderer();
  bool maintain_aspect_ratio = true;
  void DrawFrame(sf::RenderWindow& window, byte* buffer, int w, int h);
  sf::Sprite ScaleImage(byte* buffer, int sw, int sh);
};
} // natsukashii::renderer