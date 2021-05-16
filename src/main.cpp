#include <QApplication>
#include <QWidget>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
  QCoreApplication::setOrganizationName("Simone Coco");
  QCoreApplication::setApplicationName("natsukashii");
  QCoreApplication::setOrganizationDomain("https://github.com/CocoSimone/natsukashii");

  QApplication app{argc, argv};
  MainWindow window{app};
  window.show();

  return app.exec();
}
