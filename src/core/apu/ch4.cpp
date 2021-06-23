#include "ch4.h"

namespace natsukashii::core
{
CH4::CH4()
{
  nr41.raw = 0;
  nr42.raw = 0;
  nr43.raw = 0;
  nr44.raw = 0;
}

void CH4::reset()
{
  nr41.raw = 0;
  nr42.raw = 0;
  nr43.raw = 0;
  nr44.raw = 0;
}
}