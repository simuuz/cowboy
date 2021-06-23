#include "ch3.h"

namespace natsukashii::core
{
CH3::CH3()
{
  nr30.raw = 0;
  nr32.raw = 0;
  nr34.raw = 0;
  for(int i = 0; i < 16; i++) {
    wave_ram[i].raw = 0;
  }
}

void CH3::reset()
{
  nr30.raw = 0;
  nr32.raw = 0;
  nr34.raw = 0;
  for(int i = 0; i < 16; i++) {
    wave_ram[i].raw = 0;
  }
}
}