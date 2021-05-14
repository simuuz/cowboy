#include "mainwindow.h"
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QImage>

MainWindow::MainWindow(QApplication& app) : QMainWindow(nullptr)
{
  setWindowTitle("natsukashii");
  resize(800, 600);

  screen = new QImage(core->bus.ppu.pixels, 160, 144, 160, QImage::Format_RGB888);

  auto menu = new QMenuBar{this};
  auto file_menu = menu->addMenu(tr("File"));

  auto open = file_menu->addAction(tr("Open"));
  file_menu->addSeparator();
  auto exit = file_menu->addAction(tr("Exit"));

  connect(open, &QAction::triggered, this, &MainWindow::OnOpenFile);
  connect(exit, &QAction::triggered, &QApplication::quit);
  setMenuBar(menu);
  app.installEventFilter(this);
}

void MainWindow::OnOpenFile()
{
  QFileDialog file_dialog_rom{this};
  file_dialog_rom.setAcceptMode(QFileDialog::AcceptOpen);
  file_dialog_rom.setFileMode(QFileDialog::ExistingFile);
  file_dialog_rom.setNameFilter("GameBoy ROM (*.gb)");
  
  mINI::INIFile file{"config.ini"};
  mINI::INIStructure ini;

  if(!file.read(ini)) {
    ini["emulator"]["skip"] = "true";
    file.generate(ini);
  }

  bool skip = ini["emulator"]["skip"] == "true";

  if (file_dialog_rom.exec())
  {
    core = std::make_unique<natsukashii::core::Core>(
        skip, file_dialog_rom.selectedFiles().at(0).toStdString(), "bootrom.bin");

    core->Run();
  }
}