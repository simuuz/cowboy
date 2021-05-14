#include <QApplication>
#include <QWidget>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
  QApplication app{argc, argv};
  QCoreApplication::setOrganizationName("Simone Coco");

  MainWindow window{app};
  window.show();

  return app.exec();
}
