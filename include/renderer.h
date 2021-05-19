#pragma once
#include "imgui.h"
#include "imgui-sfml.h"
#include <SFML/Graphics.hpp>
#include "common.h"

namespace natsukashii::renderer
{
struct Renderer
{
  Renderer();
  bool maintain_aspect_ratio = true;
  void DrawFrame(sf::RenderWindow& window, byte* buffer, int w, int h);
  void ScaleImage(int sw, int sh);
  sf::Sprite final_image;
  sf::Texture texture;
};
} // natsukashii::renderer