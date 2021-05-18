#pragma once
#include <QApplication>
#include <QMainWindow>
#include "core.h"
#include "ini.h"
#include "renderwidget.h"

class MainWindow : public QMainWindow
{
public:
  MainWindow(QApplication& app);
private:
  void OnOpenFile();
  void closeEvent(QCloseEvent* event);
  std::unique_ptr<natsukashii::core::Core> core;
  void Reset();
  void Pause();
  void Stop();
  bool running = false;
  RenderWidget* renderer;
};