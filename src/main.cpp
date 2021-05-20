#include "mainwindow.h"

int main(int argc, char* argv[])
{
  natsukashii::frontend::MainWindow window(800, 619, "natsukashii");

  window.Run();

  return 0;
}
