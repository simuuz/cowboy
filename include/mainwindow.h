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
  void closeEvent(QCloseEvent* event);
  void OnOpenFile();
  void Quit();
  std::unique_ptr<natsukashii::core::Core> core;

  QAction* reset;
  QAction* pause;
  QAction* stop;
  RenderWidget* renderer;
};