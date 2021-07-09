#include "core.h"

namespace natsukashii::core
{
Core::Core(bool skip, std::string bootrom_path) : bus(skip, bootrom_path), cpu(skip, &bus) {}

void Core::Run(int key) {
  if(init && !pause) {
    while(cpu.total_cycles < 70224) {
      cpu.Step();
      bus.ppu.Step(cpu.cycles, bus.mem.io.intf);
      bus.apu.Step(cpu.cycles);
      bus.mem.DoInputs(key);
      cpu.HandleTimers();
    }

    cpu.total_cycles -= 70224;
  }
}

void Core::LoadROM(std::string path) {
  cpu.Reset();
  bus.Reset();
  bus.LoadROM(path);
  init = true;
}

void Core::Reset() {
  cpu.Reset();
  bus.Reset();
}

void Core::Pause() {
  pause = !pause;
}

void Core::Stop() {
  cpu.Reset();
  bus.Reset();
  init = false;
}

void Core::SaveState(int slot) {
  cpu.SaveState(slot);
}

void Core::LoadState(int slot) {
  cpu.LoadState(slot);
}
}  // namespace natsukashii::core