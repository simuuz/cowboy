#include "mainwindow.h"

namespace natsukashii::frontend
{
using clk = std::chrono::high_resolution_clock;

MainWindow::MainWindow(unsigned int w, unsigned int h, std::string title) : window(sf::VideoMode(w, h), title)
{
  ImGui::SFML::Init(window);

  mINI::INIFile file{"config.ini"};
  mINI::INIStructure ini;

  if (!file.read(ini))
  {
    ini["emulator"]["skip"] = "true";
    file.generate(ini);
  }

  bool skip = ini["emulator"]["skip"] == "true";
  std::string bootrom = ini["emulator"]["bootrom"];
  core = std::make_unique<Core>(skip, bootrom);

  NFD_Init();
}

void MainWindow::OpenFile()
{
  nfdchar_t *outpath;
  nfdfilteritem_t filteritem = { "Game Boy roms", "gb" };
  nfdresult_t result = NFD_OpenDialog(&outpath, &filteritem, 1, nullptr);
  if(result == NFD_OKAY)
  {
    core->LoadROM(std::string(outpath));
  }
}

void MainWindow::Run()
{
  sf::Clock clock;
  while(window.isOpen())
  {
    sf::Event evt;
    while(window.pollEvent(evt))
    {
      ImGui::SFML::ProcessEvent(evt);
      if(evt.type == sf::Event::Closed)
      {
        window.close();
      }
    }

    ImGui::SFML::Update(window, clock.restart());

    MenuBar();
    
    core->Run();
    if(core->bus.ppu.render)
    {
      core->bus.ppu.render = false;
      renderer.DrawFrame(window, core->bus.ppu.pixels, 800, 600);
    }
    else
    {
      window.clear();
      ImGui::SFML::Render(window);
      window.display();
    }
  }

  ImGui::SFML::Shutdown();
}

void MainWindow::MenuBar()
{
  if(ImGui::BeginMainMenuBar())
  {
    if(ImGui::BeginMenu("File"))
    {
      if(ImGui::MenuItem("Open"))
      {
        OpenFile();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

} // natsukashii::frontend