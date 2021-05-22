#include "mainwindow.h"

namespace natsukashii::frontend
{
using clk = std::chrono::high_resolution_clock;

MainWindow::MainWindow(unsigned int w, unsigned int h, std::string title)
{
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_RESIZABLE);
  renderer = new Renderer(window);
  
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
  while(running)
  {
    ImGuiIO& io = ImGui::GetIO();

		int wheel = 0;

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT) running = false;
			else if (e.type == SDL_WINDOWEVENT)
			{
				if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					io.DisplaySize.x = static_cast<float>(e.window.data1);
					io.DisplaySize.y = static_cast<float>(e.window.data2);
				}
			}
			else if (e.type == SDL_MOUSEWHEEL)
			{
				wheel = e.wheel.y;
			}
		}

    int mouseX, mouseY;
		const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
		
		io.DeltaTime = 1.0f / 60.0f;
		io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
		io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
		io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
		io.MouseWheel = static_cast<float>(wheel);

    ImGui::NewFrame();

    MenuBar();

    core->Run();
    
    if(core->bus.ppu.render) {
      core->bus.ppu.render = false;
      renderer->DrawFrame(core->bus.ppu.pixels);
    }

    ImGui::Begin("Image");
    ImGui::Image(renderer->texture, ImVec2(GB_WIDTH * 3, GB_HEIGHT * 3));
    ImGui::End();

		SDL_SetRenderDrawColor(renderer->renderer, 114, 144, 154, 255);
		SDL_RenderClear(renderer->renderer);

    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());
    SDL_RenderPresent(renderer->renderer);
  }

  ImGuiSDL::Deinitialize();
  delete renderer;
  SDL_DestroyWindow(window);
  ImGui::DestroyContext();
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