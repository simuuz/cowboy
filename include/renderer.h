#pragma once
#include "imgui.h"
#include "imgui-sfml.h"
#include <SFML/Graphics.hpp>
#include "common.h"

struct Renderer
{
  Renderer();
  bool maintain_aspect_ratio = true;
  void DrawFrame(byte* buffer, int w, int h, int sw, int sh);
};