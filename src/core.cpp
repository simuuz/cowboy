#include "core.h"

namespace natsukashii::core
{
Core::Core(bool skip, std::string bootrom_path) : bus(skip, bootrom_path), cpu(skip, &bus) { }

void Core::Run(float fps, int key, int action)
{
  if(init && running && !pause && !debug)
  {
    while(cpu.total_cycles < 70224)  // TODO: This is not proper cycling
    {
      cpu.Step();
      bus.ppu.Step(cpu.cycles, bus.mem.io.intf);
      bus.mem.DoInputs(key, action);
      cpu.bus = &bus;
      cpu.HandleTimers();
    }

    cpu.total_cycles -= 70224;
  }
}

void Core::LoadROM(std::string path)
{
  cpu.Reset();
  bus.Reset();
  bus.LoadROM(path);
  cpu.bus = &bus;
  init = true;
  running = true;
}

void Core::Reset()
{
  cpu.Reset();
  bus.Reset();
  cpu.bus = &bus;
}

void Core::Pause()
{
  pause = !pause;
}

void Core::Stop()
{
  cpu.Reset();
  bus.Reset();
  cpu.bus = &bus;
  running = false;
}

}  // namespace natsukashii::core