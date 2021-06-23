#include "core.h"

namespace natsukashii::core
{
Core::Core(bool skip, std::string bootrom_path) : bus(skip, bootrom_path), cpu(skip, &bus) {}

void Core::Run(float fps, int key, int action)
{
  if(init && !pause && !debug)
  {
    while(cpu.total_cycles < 4194300 / fps)  // TODO: This is not proper cycling
    {
      std::thread ppu(&Ppu::Step, &bus.ppu, cpu.cycles, std::ref(bus.mem.io.intf));
      std::thread apu(&Apu::Step, &bus.apu, cpu.cycles);
      cpu.Step();
      ppu.join();
      apu.join();
      bus.mem.DoInputs(key, action);
      cpu.HandleTimers();
    }

    cpu.total_cycles -= 4194300 / fps;
  }
}

void Core::LoadROM(std::string path)
{
  cpu.Reset();
  bus.Reset();
  bus.LoadROM(path);
  init = true;
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
  init = false;
}

}  // namespace natsukashii::core