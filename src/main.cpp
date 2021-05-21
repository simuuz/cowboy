#include "mainwindow.h"

int main(int argc, char* argv[])
{
  natsukashii::frontend::MainWindow window(1280, 739, "natsukashii");

  window.Run();

  return 0;
}
