#include "renderer.h"

namespace natsukashii::renderer
{
Renderer::Renderer()
{
  if (!texture.create(160, 144))
  {
    printf("couldn't create texture\n");
    exit(1);
  }

  final_image.setTexture(texture);
}

void Renderer::DrawFrame(sf::RenderWindow& window, byte* buffer, int w, int h)
{
  ScaleImage(w, h);
  texture.update(buffer, 160, 144, 0, 0);
  final_image.setTexture(texture);
  final_image.setPosition(0, 19);
  window.draw(final_image);
  ImGui::SFML::Render(window);
  window.display();
}

void Renderer::ScaleImage(int sw, int sh)
{
  float sx = sw, sy = sh;
  if(maintain_aspect_ratio)
  {
    float asp_ratio_gb = 160 / 144;
    float curr_asp_ratio = sw / sh;
    if(curr_asp_ratio > asp_ratio_gb)
    {
      sx = 160 * sh/144;
      sy = sh;
      final_image.setScale(sx / 160, sy / 144);
    }
    else
    {
      sx = sw;
      sy = 144 * sw/160;
      final_image.setScale(sx / 160, sy / 144);
    }
  }
  else
  {
    final_image.setScale(sx / 160, sy / 144);
  }
}

} // natsukashii::renderer