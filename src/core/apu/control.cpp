#include "control.h"

namespace natsukashii::core
{
Control::Control()
{
  nr50.raw = 0;
  nr51.raw = 0;
  nr52.raw = 0;
}

void Control::reset()
{
  nr50.raw = 0;
  nr51.raw = 0;
  nr52.raw = 0;
}
}