#pragma once

#include "cacti/stdlib.hpp"

namespace cacti {

namespace fontdb {
void init();
void rebuild(float scale);
void update();
void load_font(string path, float size);
void release();
}  // namespace fontdb

}  // namespace cacti