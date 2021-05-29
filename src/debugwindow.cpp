#include "debugwindow.h"

namespace natsukashii::frontend
{
	void DebugWindow::Main(Cpu& cpu, Bus& bus, bool& debug, bool& init, bool& running, float fps)
  {
    Debugger(cpu, bus, debug, init, running);
    Perf(fps);
  }

  void DebugWindow::Perf(float fps)
  {
    frame_times.add_point(1000 / fps);
    ImGui::Begin(" ", (bool*)__null, ImGuiWindowFlags_NoTitleBar);

    ImPlot::GetStyle().AntiAliasedLines = true;

    ImPlot::SetNextPlotLimitsY(0, 16, ImGuiCond_Always, 0);
    ImPlot::SetNextPlotLimitsX(0, METRIC_HISTORY_ITEMS, ImGuiCond_Always);

    if(ImPlot::BeginPlot("Performance")) {
      ImPlot::PlotLine("Frametime", frame_times.data.data(), METRIC_HISTORY_ITEMS, 1, 0, frame_times.offset);
      ImPlot::EndPlot();
    }
    ImGui::End();
  }

  void DebugWindow::Debugger(Cpu& cpu, Bus& bus, bool& debug, bool& init, bool& running)
  {
    ImGui::Begin("Debugger", (bool*)__null, ImGuiWindowFlags_NoTitleBar);
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
    if(ImGui::Button("Step") && debug && init && running) {
      cpu.Step();
      bus.ppu.Step(cpu.cycles, bus.mem.io.intf);
      cpu.HandleTimers();
      if(cpu.total_cycles >= CYCLES_PER_FRAME) {
        cpu.total_cycles -= CYCLES_PER_FRAME;
      }
    }

    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::InvisibleButton(" ", ImVec2(8, -1));
    if (ImGui::IsItemActive())
      w += ImGui::GetIO().MouseDelta.x;
    ImGui::SameLine();

    ImGui::BeginChild("child3", ImVec2(0, 0), true);
    ImGui::EndChild();
    
    ImGui::PopStyleVar();
    ImGui::End();
  }
} // natsukashii::frontend