#include "debugwindow.h"

namespace natsukashii::frontend
{
void DebugWindow::Main(Cpu& cpu, Bus& bus, bool& debug, bool& init, bool& running, float frametime)
{
  Debugger(cpu, bus, debug, init, running, 1000 / frametime);
  Perf(frametime);
}

void DebugWindow::Perf(float frametime)
{
  frame_times.add_point(frametime);
  ImGui::Begin("Profiler");

  ImPlot::GetStyle().AntiAliasedLines = true;

  ImPlot::SetNextPlotLimitsY(0, 16, ImGuiCond_Always, 0);
  ImPlot::SetNextPlotLimitsX(0, METRIC_HISTORY_ITEMS, ImGuiCond_Always);

  if(ImPlot::BeginPlot("Performance")) {
    ImPlot::PlotLine("Frametime", frame_times.data.data(), METRIC_HISTORY_ITEMS, 1, 0, frame_times.offset);
    ImPlot::EndPlot();
  }
  ImGui::End();
}

void DebugWindow::Debugger(Cpu& cpu, Bus& bus, bool& debug, bool& init, bool& pause, float fps)
{
  ImGui::Begin("Debugger");
  static float w = ImGui::GetWindowSize().x / 2;
  static float h = ImGui::GetWindowSize().y / 2 - 42;
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

  ImGui::Text("Status");

  ImGui::BeginChild("child1", ImVec2(0, h), true);

  bool Z = cpu.regs.f >> 7, N = cpu.regs.f >> 6, H = cpu.regs.f >> 5, C = cpu.regs.f >> 4;
  ImGui::Text("PC: %04X   SP: %04X", cpu.regs.pc, cpu.regs.sp);
  ImGui::Text("A: %02X   F: %02X   B: %02X   C: %02X", cpu.regs.a, cpu.regs.f, cpu.regs.b, cpu.regs.c);
  ImGui::Text("D: %02X   E: %02X   H: %02X   L: %02X", cpu.regs.d, cpu.regs.e, cpu.regs.h, cpu.regs.l);
  ImGui::Checkbox("Z   ", &Z);
  ImGui::SameLine();
  ImGui::Checkbox("N   ", &N);
  ImGui::SameLine();
  ImGui::Checkbox("H   ", &H);
  ImGui::SameLine();
  ImGui::Checkbox("C   ", &C);

  ImGui::EndChild();
  ImGui::InvisibleButton(" ", ImVec2(-1, 8));
  if (ImGui::IsItemActive())
    h += ImGui::GetIO().MouseDelta.y;

  ImGui::Text("Debugger");
  ImGui::SameLine(w + 24);
  ImGui::Text("Disassembly");
  ImGui::BeginChild("child2", ImVec2(w, 0), true);

  ImGui::Checkbox("Debug", &debug);
  if(ImGui::Button("Step") && debug && init) {
    cpu.Step();
    bus.ppu.Step(cpu.cycles, bus.mem.io.intf);
    cpu.HandleTimers();
    if(cpu.total_cycles >= 4194300 / fps) {
      cpu.total_cycles -= 4194300 / fps;
    }
  }

  ImGui::EndChild();

  ImGui::SameLine();
  ImGui::InvisibleButton(" ", ImVec2(8, -1));
  if (ImGui::IsItemActive())
    w += ImGui::GetIO().MouseDelta.x;
  ImGui::SameLine();

  ImGui::BeginChild("child3", ImVec2(0, 0), true);
  for(int i = -5; i <= 5; i++)
  {
    int pc_ = i + cpu.regs.pc;
    if(pc_ < 0)
    {
      ImGui::Text("INVALID");
    }
    else
    {
      if(init)
      {
        opcode = "%04X   " + GetInstructionDisasm(bus.ReadByte(pc_), bus.ReadByte(pc_ + 1));
        if(i == 0)
        {
          ImGui::PushStyleColor(ImGuiCol_Text, 0xFF0000FF);
        }

        auto find_byte = opcode.find("%02X");
        auto find_half = opcode.find("%04X");
        find_half = opcode.find("%04X", find_half+1);
        
        if(find_byte != std::string::npos)
          ImGui::Text(opcode.c_str(), pc_, bus.ReadByte(pc_ + 1));
        else if(find_half != std::string::npos)
          ImGui::Text(opcode.c_str(), pc_, bus.ReadHalf(pc_ + 1));
        else
          ImGui::Text(opcode.c_str(), pc_);
        
        if(i == 0)
        {
          ImGui::PopStyleColor();
        }
      }
    }
  }
  ImGui::EndChild();
  
  ImGui::PopStyleVar();
  ImGui::End();
}
} // natsukashii::frontend