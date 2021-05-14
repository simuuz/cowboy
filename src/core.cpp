#include "core.h"

constexpr int CYCLES_PER_FRAME = 4194300 / 60;

namespace natsukashii::core
{
Core::Core(bool skip, std::string path1, std::string path2) : mem(skip), bus(mem, skip, path1, path2), cpu(skip, bus) { }

void Core::Run()
{
  while(cpu.total_cycles < CYCLES_PER_FRAME)  // TODO: This is not proper cycling
  {
    cpu.step();
    bus.ppu.step(cpu.cycles);
    cpu.handle_timers();
  }

  cpu.total_cycles -= CYCLES_PER_FRAME;
}

}  // namespace natsukashii::core