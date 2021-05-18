#include "mainwindow.h"
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QImage>

MainWindow::MainWindow(QApplication& app) : QMainWindow(nullptr)
{
  auto menu = new QMenuBar{this};
  setWindowTitle("natsukashii");
  resize(800, 600);

  renderer = new RenderWidget;

  setCentralWidget(renderer);

  auto file_menu = menu->addMenu(tr("File"));

  auto open = file_menu->addAction(tr("Open"));
  file_menu->addSeparator();
  auto exit = file_menu->addAction(tr("Exit"));

  connect(open, &QAction::triggered, this, &MainWindow::OnOpenFile);
  connect(exit, &QAction::triggered, &QApplication::quit);

  auto emulation_menu = menu->addMenu(tr("Emulator"));

  auto pause = emulation_menu->addAction(tr("Pause"));
  auto reset = emulation_menu->addAction(tr("Reset"));
  auto stop = emulation_menu->addAction(tr("Stop"));

  connect(pause, &QAction::triggered, this, &MainWindow::Pause);
  connect(reset, &QAction::triggered, this, &MainWindow::Reset);
  connect(stop, &QAction::triggered, this, &MainWindow::Stop);
  setMenuBar(menu);
  app.installEventFilter(this);
}

void MainWindow::OnOpenFile()
{
  if(running)
  {
    running = false;
    core.reset();
  }

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
    running = true;
    core = std::make_unique<natsukashii::core::Core>(
        skip, file_dialog_rom.selectedFiles().at(0).toStdString(), "bootrom.bin", renderer);

    while(running)
    {
      core->Run();
      QApplication::processEvents();
    }
  }
}

void MainWindow::Stop()
{
  if(!running)
    return;
  running = false;
  core->Reset();
}

void MainWindow::Reset()
{
  if(!running)
    return;
  core->Reset();
}

void MainWindow::Pause()
{
  running = false;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  Stop();
  event->accept();
}