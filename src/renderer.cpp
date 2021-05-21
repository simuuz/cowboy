#include "renderer.h"

namespace natsukashii::renderer
{
Renderer::Renderer()
{
}

void Renderer::DrawFrame(sf::RenderWindow& window, byte* buffer, int w, int h)
{
  sf::Sprite spr(ScaleImage(buffer, w, h));
  window.draw(spr);
  ImGui::SFML::Render(window);
  window.display();
}

sf::Sprite Renderer::ScaleImage(byte* buffer, int sw, int sh)
{
  float sx = sw, sy = sh;
  if(maintain_aspect_ratio)
  {
    float asp_ratio_gb = (float)WIDTH / (float)HEIGHT;
    sx = sh / asp_ratio_gb;
    sy = sx / asp_ratio_gb;
  }

  sf::Texture tex;
  if (!tex.create(WIDTH, HEIGHT))
  {
    printf("couldn't create texture\n");
    exit(1);
  }
  tex.update(buffer, WIDTH, HEIGHT, 0, 0);
  sf::Sprite res;
  res.setTexture(tex);
  res.setScale(sx / WIDTH, sy / HEIGHT);
  res.setPosition(sw / 2 - (sx / 2), sh / 2 - (sy / 2));
  return res;
}

} // natsukashii::renderer