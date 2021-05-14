#pragma once
#include <QApplication>
#include <QMainWindow>
#include "core.h"
#include "../dependencies/mINI/src/mini/ini.h"

class MainWindow : public QMainWindow
{
public:
  MainWindow(QApplication& app);

private:
  void OnOpenFile();
  std::unique_ptr<natsukashii::core::Core> core;
  QAction* reset;
  QAction* pause;
  QAction* stop;
  QImage* screen;
};