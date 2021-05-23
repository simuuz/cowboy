#include "mainwindow.h"

namespace natsukashii::frontend
{
using clk = std::chrono::high_resolution_clock;

MainWindow::MainWindow(unsigned int w, unsigned int h, std::string title)
{
  glfwSetErrorCallback(glfw_error_callback);
  if(!glfwInit())
    exit(1);

#if defined(IMGUI_IMPL_OPENGL_ES2)
  const char* glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif
  window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (glewInit() != GLEW_OK)
  {
      fprintf(stderr, "Failed to initialize OpenGL loader!\n");
      exit(1);
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

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

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, core->bus.ppu.pixels);
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
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, core->bus.ppu.pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void MainWindow::Run()
{
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  while(!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    core->Run();
    if(core->bus.ppu.render) {
      core->bus.ppu.render = false;
      UpdateTexture();
    }

    ImGui::Begin("Image");

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

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  
  glfwDestroyWindow(window);
  glfwTerminate();
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