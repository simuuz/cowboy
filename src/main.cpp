#include "mainwindow.h"

int main(int, char**)
{
  natsukashii::frontend::MainWindow window(1280, 720, "natsukashii");

  window.Run();

  return 0;
}
