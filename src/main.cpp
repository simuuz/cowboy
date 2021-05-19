#include "mainwindow.h"

int main(int argc, char* argv[])
{
  natsukashii::frontend::MainWindow window(800, 600, "natsukashii");

  window.Run();

  return 0;
}
