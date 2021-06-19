#include "core.h"

namespace natsukashii::core
{
Core::Core(bool skip, std::string bootrom_path) : bus(skip, bootrom_path), cpu(&scheduler, skip, &bus) { }

void Core::Run(float fps, int key, int action)
{
  if(init && running && !pause && !debug)
  {
    while(cpu.timestamp <= scheduler.entries[0].time)
    {
      cpu.Step();
      scheduler = *cpu.scheduler;
      bus.mem.DoInputs(key, action);
      cpu.bus = &bus;
      cpu.HandleTimers();
    }

    ProcessPendingEvents();
  }
}

void Core::ProcessPendingEvents() {
  int last_pos = 0;
  for(int i = 0; i < scheduler.pos; i++) {
    if(scheduler.entries[i].time > cpu.timestamp) {
      last_pos = i;
      break;
    }

    switch (scheduler.entries[i].event)
    {
    case Event::PPU:
      bus.ppu.OnEvent(scheduler.entries[i], &scheduler, bus.mem.io.intf);
      break;
    }
  }
  
  scheduler.pop(last_pos);
  cpu.scheduler = &scheduler;
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