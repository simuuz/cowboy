#include "mainwindow.h"
#include <sstream>

namespace natsukashii::frontend
{
using clk = std::chrono::high_resolution_clock;

MainWindow::~MainWindow()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
  
  glfwDestroyWindow(window);
  glfwTerminate();
  NFD_Quit();
}

MainWindow::MainWindow(std::string title)
{
  glfwSetErrorCallback(glfw_error_callback);
  if(!glfwInit())
  {
    running = false;
    core.reset();
    exit(1);
  }

  const char* glsl_version = "#version 450";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

  const GLFWvidmode *details = glfwGetVideoMode(glfwGetPrimaryMonitor());
  int w = details->width - (details->width / 4), h = details->height - (details->height / 4);
  window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
  glfwSetWindowPos(window, details->width / 2 - w / 2, details->height / 2 - h / 2);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);

  if (glewInit() != GLEW_OK)
  {
    fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    exit(1);
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  mINI::INIFile file{"config.ini"};
  mINI::INIStructure ini;

  if (!file.read(ini))
  {
    ini["emulator"]["skip"] = "false";
    ini["emulator"]["bootrom"] = "bootrom.bin";
    ini["palette"]["color1"] = "e0f8d0ff";
    ini["palette"]["color2"] = "88c070ff";
    ini["palette"]["color3"] = "346856ff";
    ini["palette"]["color4"] = "81820ff";
    file.generate(ini);
  }

  bool skip = ini["emulator"]["skip"] == "true";
  std::string bootrom = ini["emulator"]["bootrom"];
  core = std::make_unique<Core>(skip, bootrom);

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, core->bus.ppu.pixels.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

void MainWindow::UpdateTexture()
{
  glBindTexture(GL_TEXTURE_2D, id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, core->bus.ppu.pixels.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void MainWindow::Run()
{
  int i = 0;
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  while(!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    core->Run(io.Framerate);

    if(core->bus.ppu.render) {
      core->bus.ppu.render = false;
      UpdateTexture();
    }

    i++;
    if(i >= 1000) {
      i = 0;
      char title[50]{0};
      sprintf(title, "natsukashii [%.2f fps | %.2f ms]", io.Framerate, 1000 / io.Framerate);
      glfwSetWindowTitle(window, title);
    }

    if(show_debug_windows)
      dbg.Main(core->cpu, core->bus, core->debug, core->init, core->running, io.Framerate);

    if(show_settings)
      Settings();

    ImGui::Begin("Image", (bool*)__null, ImGuiWindowFlags_NoTitleBar);

    float x = ImGui::GetWindowSize().x - 15, y = ImGui::GetWindowSize().y - 15;
    float current_aspect_ratio = x / y;
    if(aspect_ratio_gb > current_aspect_ratio) {
      y = x / aspect_ratio_gb;
    } else {
      x = y * aspect_ratio_gb;
    }

    ImVec2 image_size(x, y);
    ImVec2 centered((ImGui::GetWindowSize().x - image_size.x) * 0.5, (ImGui::GetWindowSize().y - image_size.y) * 0.5);
    ImGui::SetCursorPos(centered);
    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(id)), image_size);
    ImGui::End();

    MenuBar();
    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
		glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }
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

      if(ImGui::MenuItem("Exit"))
      {
        running = false;
        core.reset();
        glfwSetWindowShouldClose(window, GLFW_TRUE);
      }
      ImGui::EndMenu();
    }
    
    if(ImGui::BeginMenu("Emulation"))
    {
      if(ImGui::MenuItem(core->pause ? "Resume" : "Pause"))
      {
        core->Pause();
      }

      if(ImGui::MenuItem("Reset"))
      {
        core->Reset();
      }

      if(ImGui::MenuItem("Stop"))
      {
        core->Stop();
      }

      bool settings_clicked = ImGui::MenuItem("Settings");
      if(ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Any setting needs restart to take effect");
      }
      if(settings_clicked)
      {
        show_settings = true;
      }

      ImGui::Checkbox("Show debug windows", &show_debug_windows);

      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void MainWindow::Settings()
{
  mINI::INIFile file{"config.ini"};
  mINI::INIStructure ini;

  if (!file.read(ini))
  {
    ini["emulator"]["skip"] = "false";
    ini["emulator"]["bootrom"] = "bootrom.bin";
    ini["palette"]["color1"] = "e0f8d0ff";
    ini["palette"]["color2"] = "88c070ff";
    ini["palette"]["color3"] = "346856ff";
    ini["palette"]["color4"] = "81820ff";
    file.generate(ini);
  }

  bool skip = ini["emulator"]["skip"] == "true";
  std::string bootrom = ini["emulator"]["bootrom"];

  static bool general = true;
  static bool graphics = false;
  ImGui::Begin("Settings", &show_settings);
  static float w = ImGui::GetWindowSize().x / 4;
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

  ImGui::BeginChild("child1", ImVec2(w, 0), true);
  if(ImGui::Button("General")) {
    general = true;
    graphics = false;
  }
  ImGui::NewLine();
  if(ImGui::Button("Graphics")) {
    general = false;
    graphics = true;
  }
  ImGui::EndChild();

  ImGui::SameLine();
  ImGui::InvisibleButton(" ", ImVec2(8, -1));
  if (ImGui::IsItemActive())
    w += ImGui::GetIO().MouseDelta.x;
  ImGui::SameLine();

  ImGui::BeginChild("child2", ImVec2(0, 0), true);
  if(general)
  {
    ImGui::Text("Bootrom path: \"%s\"", bootrom.c_str());
    
    if(ImGui::Button("Select...")) {
      nfdchar_t *outpath;
      nfdresult_t result = NFD_OpenDialog(&outpath, nullptr, 0, nullptr);
      if(result == NFD_OKAY) {
        ini["emulator"]["bootrom"] = outpath;
        file.write(ini);
      }
    }
    static const char* skip_ = "Skip bootrom: ";
    ImGui::Text(skip_);
    ImGui::SameLine(ImGui::CalcTextSize(skip_).x + 20);
    if(ImGui::Checkbox(" ", &skip)) {
      ini["emulator"]["skip"] = skip ? "true" : "false";
      file.write(ini);
    }
  }
  else if (graphics)
  {    
    float color1[4] = {(float)(std::stoul(ini["palette"]["color1"].c_str(), nullptr, 16) >> 24) / 255,
                       (float)((std::stoul(ini["palette"]["color1"].c_str(), nullptr, 16) >> 16) & 0xFF) / 255,
                       (float)((std::stoul(ini["palette"]["color1"].c_str(), nullptr, 16) >> 8) & 0xFF) / 255,
                       (float)(std::stoul(ini["palette"]["color1"].c_str(), nullptr, 16) & 0xFF) / 255};

    float color2[4] = {(float)(std::stoul(ini["palette"]["color2"].c_str(), nullptr, 16) >> 24) / 255,
                       (float)((std::stoul(ini["palette"]["color2"].c_str(), nullptr, 16) >> 16) & 0xFF) / 255,
                       (float)((std::stoul(ini["palette"]["color2"].c_str(), nullptr, 16) >> 8) & 0xFF) / 255,
                       (float)(std::stoul(ini["palette"]["color2"].c_str(), nullptr, 16) & 0xFF) / 255};

    float color3[4] = {(float)(std::stoul(ini["palette"]["color3"].c_str(), nullptr, 16) >> 24) / 255,
                       (float)((std::stoul(ini["palette"]["color3"].c_str(), nullptr, 16) >> 16) & 0xFF) / 255,
                       (float)((std::stoul(ini["palette"]["color3"].c_str(), nullptr, 16) >> 8) & 0xFF) / 255,
                       (float)(std::stoul(ini["palette"]["color3"].c_str(), nullptr, 16) & 0xFF) / 255};

    float color4[4] = {(float)(std::stoul(ini["palette"]["color4"].c_str(), nullptr, 16) >> 24) / 255,
                       (float)((std::stoul(ini["palette"]["color4"].c_str(), nullptr, 16) >> 16) & 0xFF) / 255,
                       (float)((std::stoul(ini["palette"]["color4"].c_str(), nullptr, 16) >> 8) & 0xFF) / 255,
                       (float)(std::stoul(ini["palette"]["color4"].c_str(), nullptr, 16) & 0xFF) / 255};

    ImGui::Text("Customize the palette");
    ImGui::ColorEdit4("Color 1", color1);
    ImGui::ColorEdit4("Color 2", color2);
    ImGui::ColorEdit4("Color 3", color3);
    ImGui::ColorEdit4("Color 4", color4);

    std::stringstream sstream;

    word color1_ = word((byte(255 * color1[0]) << 24) | (byte(255 * color1[1]) << 16) | (byte(255 * color1[2]) << 8) | byte(255 * color1[3]));
    word color2_ = word((byte(255 * color2[0]) << 24) | (byte(255 * color2[1]) << 16) | (byte(255 * color2[2]) << 8) | byte(255 * color2[3]));
    word color3_ = word((byte(255 * color3[0]) << 24) | (byte(255 * color3[1]) << 16) | (byte(255 * color3[2]) << 8) | byte(255 * color3[3]));
    word color4_ = word((byte(255 * color4[0]) << 24) | (byte(255 * color4[1]) << 16) | (byte(255 * color4[2]) << 8) | byte(255 * color4[3]));

    core->cpu.bus->ppu.color1 = color1_;
    core->cpu.bus->ppu.color2 = color2_;
    core->cpu.bus->ppu.color3 = color3_;
    core->cpu.bus->ppu.color4 = color4_;

    sstream << std::hex << color1_ << "\n";
    sstream << std::hex << color2_ << "\n";
    sstream << std::hex << color3_ << "\n";
    sstream << std::hex << color4_ << "\n";

    sstream >> ini["palette"]["color1"] >> ini["palette"]["color2"] >> ini["palette"]["color3"] >> ini["palette"]["color4"];
    file.write(ini);
  }
  ImGui::EndChild();

  ImGui::PopStyleVar();
  ImGui::End();
}

} // natsukashii::frontend