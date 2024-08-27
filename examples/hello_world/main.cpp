#include <SDL2/SDL_main.h>

#include <cstdio>

#include "cacti/application.hpp"
#include "cacti/font_database.hpp"
#include "imgui.h"

using namespace cacti;

void on_init();
void on_gui();
void load_fonts();

int main(int argc, char* argv[]) {
    /* clang-format off */ 
    app_init_params initParams = {
        .argc = argc,
        .argv = argv,
        .window = {
            .center = false,
            .size = {1024, 768},
            .minSize = {1024, 768},
            .startMaximized = true,
         },
        .callbacks = {
            .imguiSetup = &on_init,
            .loadFonts = &load_fonts,
            .gui = &on_gui
        }
    };
    /* clang-format on */

    auto result = run_app(initParams);
    if (result.failed()) {
        std::fprintf(
            stderr, "Run error @ %s:%d:%d: %s\n",
            result.error().where().file_name(), result.error().where().line(),
            result.error().where().column(), result.error().what().c_str());
        return 1;
    }

    return 0;
}

void on_init() {
    ImGui::StyleColorsLight();

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard |
                      ImGuiConfigFlags_ViewportsEnable |
                      ImGuiConfigFlags_DockingEnable;
    io.ConfigDockingWithShift = true;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    auto& style = ImGui::GetStyle();
    style.FrameRounding = 2;
    style.WindowMenuButtonPosition = ImGuiDir_Right;
}

void load_fonts() { fontdb::load_font("NotoSans-Regular.ttf", 16.0f); }

void on_gui() {
    ImGui::DockSpaceOverViewport();
    ImGui::ShowDemoWindow();
}