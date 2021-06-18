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
  for(auto& entry: scheduler.entries) {
    if(entry.time > cpu.timestamp) {
      break;
    }

    switch (entry.event)
    {
    case Event::PPU:
      bus.ppu.OnEvent(entry, &scheduler, bus.mem.io.intf);
      break;
    }

    scheduler.pop();
  }

  scheduler.entries[scheduler.pos - 1].time = UINT64_MAX;
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