#include "mainwindow.h"
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>

MainWindow::MainWindow(QApplication& app) : QMainWindow(nullptr)
{
  setWindowTitle("natsukashii");
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
  QFileDialog file_dialog_rom{this};
  file_dialog_rom.setAcceptMode(QFileDialog::AcceptOpen);
  file_dialog_rom.setFileMode(QFileDialog::ExistingFile);
  file_dialog_rom.setNameFilter("GameBoy ROM (*.gb)");
  if (file_dialog_rom.exec())
  {
    mINI::INIFile file{"config.ini"};
    mINI::INIStructure ini;
    file.read(ini);
    bool skip = ini["emulator"]["skip"] == "true";
    std::string bootrom = ini["emulator"]["bootrom"];
    if (bootrom.empty())
    {
      QFileDialog file_dialog_bootrom{this};
      file_dialog_bootrom.setAcceptMode(QFileDialog::AcceptOpen);
      file_dialog_bootrom.setFileMode(QFileDialog::ExistingFile);
      bootrom = file_dialog_bootrom.selectedFiles().at(0).toStdString();
      ini["emulator"]["bootrom"] = bootrom;
      file.write(ini);
    }

    core = std::make_unique<natsukashii::core::Core>(
        skip, file_dialog_rom.selectedFiles().at(0).toStdString(), bootrom);

    core->Run();
  }
}