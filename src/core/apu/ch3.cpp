#include "ch3.h"

namespace natsukashii::core
{
CH3::CH3()
{
  nr30.raw = 0;
  nr32.raw = 0;
  nr34.raw = 0;
  memset(wave_ram, 0, 16);
}

void CH3::reset()
{
  nr30.raw = 0;
  nr32.raw = 0;
  nr34.raw = 0;
  memset(wave_ram, 0, 16);
}
}