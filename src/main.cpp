#include <iostream>
#include <filesystem>
#include <chrono>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "tinyfiledialogs.h"
#include "ini.h"
#include "cpu.h"
#include "mem.h"

using cl_hires = std::chrono::high_resolution_clock;

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    std::string title = "Cowboy";
    SDL_Window* window = SDL_CreateWindow("Cowboy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * 3, 144 * 3 + 19, SDL_WINDOW_OPENGL);
    
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(0);

    if(glewInit() != GLEW_OK) {
        printf("Failed to initialize OpenGL loader!\n");
        return 1;
    }

    Mem mem;
    Cpu cpu;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    SDL_Event event;
    bool quit = false;
    bool pause = false;
    std::filesystem::path rom;
    while(!quit) {
        auto start = cl_hires::now();
        while(SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch(event.type) {
                case SDL_QUIT: quit = true; break;
                case SDL_WINDOWEVENT:
                if(event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                    quit = true;
                break;
                case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_p:
                    if(pause && mem.rom_opened) {
                        cpu.step();
                    }
                    break;
                }
                break;
            }
        }

        for(int i = 0; !pause && mem.rom_opened && i < 69905; i++) {
            cpu.step();
        }
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        if(ImGui::BeginMainMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("Open rom")) {
                    mem.reset();
                    cpu.reset();
                    mINI::INIFile file("config.ini");
                    mINI::INIStructure ini;
                    file.read(ini);
                    std::string bootrom = ini["emulator"]["bootrom"];
                    if(bootrom.empty()) {
                        bootrom = tinyfd_openFileDialog("This is a one-time thing. You need to select a bootrom file and I'll remember it for you.",
                                (std::filesystem::current_path().string() + "/").c_str(), 0, nullptr, "Valid GameBoy bootrom", 0);
                        ini["emulator"]["bootrom"] = bootrom;
                        file.write(ini);
                    }
                    mem.loadBootROM(bootrom);
                    char const* filter = "*.gb";
                    const char* rom_charstr = tinyfd_openFileDialog("Select a GameBoy rom",
                                (std::filesystem::current_path().string() + "/").c_str(), 1, &filter, "Valid GameBoy rom", 0);

                    if(rom_charstr != nullptr) {
                        rom = rom_charstr;
                        mem.loadROM(rom.string());
                        cpu.mem = mem;
                    }
                }
                if(ImGui::MenuItem("Exit")) { quit = true; }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Emulation")) {
                if(ImGui::MenuItem((pause) ? "Resume" : "Pause")) { pause = !pause; }
                if(ImGui::MenuItem("Stop")) {
                    cpu.reset();
                    mem.reset();
                    cpu.mem = mem;
                }
                if(ImGui::MenuItem("Reset") && !rom.empty()) {
                    cpu.reset();
                    mem.reset();
                    mem.loadROM(rom.string());
                    cpu.mem = mem;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);

        float frametime = std::chrono::duration<float, std::milli>(cl_hires::now() - start).count();
        char fps_frametime[32];
        snprintf(fps_frametime, 32, " | %.2f fps | %.2f ms", 1000 / frametime, frametime);
        SDL_SetWindowTitle(window, (mem.rom_opened) ? (title + " - \"" + rom.stem().string() + "\"" + fps_frametime).c_str()
                                                    : (title + " - Nothing playing" + fps_frametime).c_str());
        SDL_Delay(1);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}