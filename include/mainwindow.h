#pragma once
#include "core.h"
#include "ini.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <nfd.hpp>
#include <chrono>

namespace natsukashii::frontend
{
using namespace natsukashii::core;

static void glfw_error_callback(int error, const char* description)
{
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

constexpr float aspect_ratio_gb = (float)WIDTH / (float)HEIGHT;
struct MainWindow
{
  MainWindow(unsigned int w, unsigned h, std::string title);
  ~MainWindow() { NFD_Quit(); }
  void Run();
  void OpenFile();
  void Reset();
  void Pause();
  void Stop();
  void MenuBar();
  void UpdateTexture();
  bool running = true;
  GLFWwindow* window = nullptr;
  std::unique_ptr<Core> core;
  unsigned int id = 0;
};
} // natsukashii::frontend