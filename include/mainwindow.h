#pragma once
#include "core.h"
#include <nfd.hpp>

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
  void GeneralSettings(mINI::INIFile& file, mINI::INIStructure& ini, bool& skip, std::string& bootrom);
  void GraphicsSettings(mINI::INIFile& file, mINI::INIStructure& ini);
  bool running = true;
  bool show_palette = false;
  bool show_debug_windows = false;
  bool show_settings = false;
  mINI::INIFile file;
  mINI::INIStructure ini;
  GLFWwindow* window = nullptr;
  std::unique_ptr<Core> core;
  unsigned int id = 0;
  DebugWindow dbg;
};
} // natsukashii::frontend