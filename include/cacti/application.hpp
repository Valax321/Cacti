#pragma once

#include "cacti/error.hpp"
#include "cacti/math.hpp"
#include "cacti/result.hpp"
#include "cacti/stdlib.hpp"

union SDL_Event;

namespace cacti {

using app_init_result = result<nullptr_t, error>;
using app_run_result = result<nullptr_t, error>;

struct window_init_params {
    bool center{};
    size32 size{};
    size32 minSize{};
    bool startMaximized{};
    bool allowResize{true};
};

struct app_callbacks {
    std::function<void()> imguiSetup{};
    std::function<void()> loadFonts{};
    std::function<void()> quit{};
    std::function<void(SDL_Event*)> eventProcess{};
    std::function<void()> gui{};
};

struct app_init_params {
    int argc{};
    const char* const* argv{};
    window_init_params window{};
    app_callbacks callbacks{};
};

CACTI_EXPORT app_init_result init(const app_init_params& params);
CACTI_EXPORT void quit();
CACTI_EXPORT void run();

[[nodiscard]] CACTI_EXPORT bool is_initialized() noexcept;
[[nodiscard]] CACTI_EXPORT bool is_running() noexcept;

inline app_run_result run_app(const app_init_params& params) {
    if (auto initResult = init(params); initResult.failed())
        return {initResult.error()};

    while (is_running()) run();

    quit();
    return {nullptr};
}

}  // namespace cacti
