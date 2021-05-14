#include <chrono>
#include <filesystem>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
  QApplication app{argc, argv};

  QCoreApplication::setOrganizationName("simuuz");
  QCoreApplication::setOrganizationName("shibumi");

  MainWindow window{app};

  window.show();

  return app.exec();
}