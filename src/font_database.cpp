#include "cacti/font_database.hpp"

#include "application_internal.hpp"
#include "imgui.h"

namespace cacti {
namespace fontdb {

struct font_defintion {
   public:
    font_defintion(string path, float size) : _path(path), _size(size) {
        auto io = ImGui::GetIO();
        rebuild(io.DisplayFramebufferScale.x);
    }

    void rebuild(float scale) {
        auto io = ImGui::GetIO();
        _font = io.Fonts->AddFontFromFileTTF(_path.c_str(), _size * scale);
    }

    [[nodiscard]] string name() const {
        if (_font != nullptr) {
            return _font->ConfigData->Name;
        }

        return string();
    }

   private:
    string _path{};
    ImFont* _font{};
    float _size{};
};

struct font_db_state {
    std::vector<font_defintion*> fonts{};
    float lastScale{1};
};

static font_db_state db_state{};

void init() { db_state.lastScale = ImGui::GetIO().DisplayFramebufferScale.x; }

void load_font(string path, float size) {
    auto* font = new font_defintion(path, size);
    db_state.fonts.push_back(font);
}

void rebuild(float scale) {
    auto io = ImGui::GetIO();
    io.Fonts->Clear();
    if (db_state.fonts.size() == 0) io.Fonts->AddFontDefault();

    for (auto font : db_state.fonts) {
        font->rebuild(scale);
    }

    io.Fonts->Build();
    rebuild_font_texture();
}

void update() {
    auto io = ImGui::GetIO();
    auto currentScale = io.DisplayFramebufferScale.x;
    if (db_state.lastScale != currentScale) {
        rebuild(currentScale);
        db_state.lastScale = currentScale;
    }
}

void release() {
    for (auto font : db_state.fonts) {
        delete font;
    }

    db_state.fonts.clear();
}

}  // namespace fontdb
}  // namespace cacti