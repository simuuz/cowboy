#include "mainwindow.h"
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>

constexpr int CYCLES_PER_FRAME = 4194300 / (1000 / DELAY);

MainWindow::MainWindow(QApplication& app) : QMainWindow(nullptr)
{
  setWindowTitle("shibumi");
  resize(800, 600);

  auto menu = new QMenuBar{this};
  auto file_menu = menu->addMenu(tr("File"));

  auto open = file_menu->addAction(tr("Open"));
  file_menu->addSeparator();
  auto exit = file_menu->addAction(tr("Close"));

  connect(open, &QAction::triggered, this, &MainWindow::OnOpenFile);
  connect(exit, &QAction::triggered, &QApplication::quit);
  setMenuBar(menu);
  app.installEventFilter(this);
}

void MainWindow::OnOpenFile()
{
  QFileDialog file_dialog{this};
  file_dialog.setAcceptMode(QFileDialog::AcceptOpen);
  file_dialog.setFileMode(QFileDialog::ExistingFile);
  file_dialog.setNameFilter("GameBoy ROM (*.gb)");
  if (file_dialog.exec())
  {
    mINI::INIFile file{"config.ini"};
    mINI::INIStructure ini;
    file.read(ini);
    bool skip = ini["emulator"]["skip"] == "true";
    std::string bootrom = ini["emulator"]["bootrom"];
    if (bootrom.empty())
    {
      bootrom = file_dialog.selectedFiles().at(0).toStdString();
      ini["emulator"]["bootrom"] = bootrom;
      file.write(ini);
    }

    core = std::make_unique<Cpu>(skip, file_dialog.selectedFiles().at(0).toStdString(), bootrom);

    while (core->total_cycles < CYCLES_PER_FRAME)
    {
      core->Run();
      core->bus.ppu.step(core->cycles);
      core->handle_timers();
    }

    core->total_cycles -= CYCLES_PER_FRAME;
  }
}