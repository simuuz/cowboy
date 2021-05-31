#include "core.h"

namespace natsukashii::core
{
Core::Core(bool skip, std::string bootrom_path) : bus(skip, bootrom_path), cpu(skip, &bus) { }

void Core::Run(float fps)
{
  if((init = bus.mem.rom_opened) && running && !pause && !debug)
  {
    while(cpu.total_cycles < 4194300 / fps)  // TODO: This is not proper cycling
    {
      cpu.Step();
      bus.ppu.Step(cpu.cycles, bus.mem.io.intf);
      cpu.HandleTimers();
    }

    cpu.total_cycles -= 4194300 / fps;
  }
}

void Core::LoadROM(std::string path)
{
  cpu.Reset();
  bus.LoadROM(path);
  init = true;
  running = true;
}

void Core::Reset()
{
  cpu.Reset();
  bus.Reset();
}

void Core::Pause()
{
  pause = !pause;
}

void Core::Stop()
{
  cpu.Reset();
  bus.Reset();
  running = false;
}

}  // namespace natsukashii::core