#include "cacti/application.hpp"

#include <SDL2/SDL.h>

#include "application_internal.hpp"
#include "cacti/font_database.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"

#if defined(CACTI_BACKEND_METAL)
#include "imgui_impl_metal.h"
#include "metal_interop.h"
#elif defined(CACTI_BACKEND_OPENGL)
#include <SDL2/SDL_opengl.h>

#include "imgui_impl_opengl3.h"
constexpr const char* GLSL_VERSION = "#version 150";
#endif

namespace cacti {

struct app_state {
    SDL_Window* mainWindow{};
#if defined(CACTI_BACKEND_METAL)
    SDL_MetalView view{};
#elif defined(CACTI_BACKEND_OPENGL)
    SDL_GLContext glContext{};
#endif
    bool initialized{};
    bool isRunning{};

    app_callbacks callbacks{};
};

static app_state state{};

void init_imgui(const app_init_params& params);
app_init_result init_graphics_backend(const app_init_params& params);
void run_event_loop();
void setup_frame();
void do_gui_layout();
void end_frame();

app_init_result init(const app_init_params& params) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        return {MAKE_ERROR_HERE(SDL_GetError())};
    }

    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    if (auto r = init_graphics_backend(params); r.failed()) {
        return r;
    }

    init_imgui(params);

    fontdb::init();
    if (params.callbacks.loadFonts) {
        params.callbacks.loadFonts();
    }

    state.callbacks = params.callbacks;
    state.initialized = true;
    state.isRunning = true;
    return {nullptr};
}

void init_imgui(const app_init_params& params) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;

    if (params.callbacks.imguiSetup) {
        params.callbacks.imguiSetup();
    }

#if defined(CACTI_BACKEND_OPENGL)
    ImGui_ImplSDL2_InitForOpenGL(state.mainWindow, state.glContext);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);
#endif
}

app_init_result init_graphics_backend(const app_init_params& params) {
    s32 x = SDL_WINDOWPOS_UNDEFINED;
    s32 y = x;

    if (params.window.center) {
        x = y = SDL_WINDOWPOS_CENTERED;
    }

    u32 windowFlags = SDL_WINDOW_ALLOW_HIGHDPI;

#if defined(CACTI_BACKEND_METAL)
    windowFlags |= SDL_WINDOW_METAL;
#elif defined(CACTI_BACKEND_OPENGL)
    windowFlags |= SDL_WINDOW_OPENGL;
#if __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
#endif

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
#endif

    if (params.window.startMaximized) {
        windowFlags |= SDL_WINDOW_MAXIMIZED;
    }

    if (params.window.allowResize) {
        windowFlags |= SDL_WINDOW_RESIZABLE;
    }

    state.mainWindow =
        SDL_CreateWindow("Cacti App", x, y, params.window.size.width,
                         params.window.size.height, windowFlags);

    if (state.mainWindow == nullptr) {
        return {MAKE_ERROR_HERE(SDL_GetError())};
    }

#if defined(CACTI_BACKEND_METAL)
    state.view = SDL_Metal_CreateView(state.mainWindow);
    if (state.view == nullptr) {
        return {MAKE_ERROR_HERE(SDL_GetError())};
    }
    auto* layer = GetMetalLayerFromSDL(state.view);
    layer->setDevice(MTL::CreateSystemDefaultDevice());
    ImGui_ImplMetal_Init(layer->device());
    ImGui_ImplSDL2_InitForMetal(state.mainWindow);
#elif defined(CACTI_BACKEND_OPENGL)
    state.glContext = SDL_GL_CreateContext(state.mainWindow);
    if (state.glContext == nullptr) {
        return {MAKE_ERROR_HERE(SDL_GetError())};
    }

    SDL_GL_MakeCurrent(state.mainWindow, state.glContext);
    SDL_GL_SetSwapInterval(1);
#endif

    return {nullptr};
}

void quit() {
    if (state.callbacks.quit) {
        state.callbacks.quit();
    }

    fontdb::release();

#if defined(CACTI_BACKEND_OPENGL)
    ImGui_ImplOpenGL3_Shutdown();
    SDL_GL_DeleteContext(state.glContext);
    state.glContext = nullptr;
#elif defined(CACTI_BACKEND_METAL)
    ImGui_ImplMetal_Shutdown();
#endif
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyWindow(state.mainWindow);
    state.mainWindow = nullptr;
    SDL_Quit();
    state.initialized = false;
}

void run() {
    if (!state.initialized) return;

    // auto flags = SDL_GetWindowFlags(state.mainWindow);
    // if (!(flags & SDL_WINDOW_HIDDEN)) {
    //     SDL_ShowWindow(state.mainWindow);
    // }

    run_event_loop();
    setup_frame();
    do_gui_layout();
    end_frame();
}

void run_event_loop() {
    SDL_Event ev{};
    while (SDL_PollEvent(&ev)) {
        ImGui_ImplSDL2_ProcessEvent(&ev);
        if (state.callbacks.eventProcess) {
            state.callbacks.eventProcess(&ev);
        }
        if (ev.type == SDL_QUIT) state.isRunning = false;
    }
}

void setup_frame() {
#if defined(CACTI_BACKEND_OPENGL)
    ImGui_ImplOpenGL3_NewFrame();
#endif
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void end_frame() {
    auto& io = ImGui::GetIO();
    auto color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
    ImGui::Render();
#if defined(CACTI_BACKEND_OPENGL)
    glViewport(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    glClearColor(color.x, color.y, color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
        SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }

    io.FontGlobalScale = 1.0f / io.DisplayFramebufferScale.x;
    fontdb::update();

    SDL_GL_SwapWindow(state.mainWindow);
#endif
}

void do_gui_layout() {
    if (state.callbacks.gui) {
        state.callbacks.gui();
    }
}

void rebuild_font_texture() {
#if defined(CACTI_BACKEND_OPENGL)
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_CreateFontsTexture();
#endif
}

bool is_initialized() noexcept { return state.initialized; }
bool is_running() noexcept { return state.isRunning; }

}  // namespace cacti