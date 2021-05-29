#pragma once
#include "core.h"
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
  MainWindow(std::string title);
  ~MainWindow();
  void Run();
  void OpenFile();
  void MenuBar();
  void UpdateTexture();
  void Settings();
  bool running = true;
  bool show_palette = false;
  bool show_debug_windows = true;
  bool show_settings = false;
  ImVec2 display_size;
  ImVec2 debug_size;
  ImVec2 perf_size;
  GLFWwindow* window = nullptr;
  std::unique_ptr<Core> core;
  unsigned int id = 0;
  DebugWindow dbg;
};
} // natsukashii::frontend