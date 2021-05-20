#include "core.h"

constexpr int CYCLES_PER_FRAME = 4194300 / 60;

namespace natsukashii::core
{
Core::Core(bool skip, std::string bootrom_path) : mem(skip), bus(mem, skip, bootrom_path), cpu(skip, bus) { }

void Core::Run()
{
  if(canrun)
  {
    while(cpu.total_cycles < CYCLES_PER_FRAME)  // TODO: This is not proper cycling
    {
      cpu.Step();
      cpu.bus.ppu.Step(cpu.cycles);
      cpu.HandleTimers();
    }

    cpu.total_cycles -= CYCLES_PER_FRAME;
  }
}

void Core::LoadROM(std::string path)
{
  bus.LoadROM(path);
  canrun = bus.romopened;
}

void Core::Reset()
{
  bus.Reset();
  cpu.Reset();
}

void Core::Pause()
{
  canrun = false;
}

void Core::Stop()
{
  bus.Reset();
  cpu.Reset();
  canrun = false;
}

}  // namespace natsukashii::core