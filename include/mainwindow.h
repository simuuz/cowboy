#pragma once
#include <QApplication>
#include <QMainWindow>
#include "cpu.h"
#include "ini.h"

class MainWindow : public QMainWindow
{
public:
  MainWindow(QApplication& app);

private:
  void OnOpenFile();
  std::unique_ptr<Cpu> core;
  QAction* reset;
  QAction* pause;
  QAction* stop;
};