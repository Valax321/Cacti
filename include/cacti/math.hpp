#pragma once

#include "cacti/stdlib.hpp"

namespace cacti {

template <typename unit_type>
struct size {
   public:
    size() {}
    size(const unit_type width, const unit_type height)
        : width(width), height(height) {}

    size operator+(const size& other) {
        return {width + other.width, height + other.height};
        return *this;
    }

    size operator-(const size& other) {
        return {width - other.width, height - other.height};
        return *this;
    }

    size operator*(const unit_type scale) {
        return {width * scale, height * scale};
        return *this;
    }

    size operator/(const unit_type scale) {
        return {width / scale, height / scale};
        return *this;
    }

    unit_type width{};
    unit_type height{};
};

using size32 = size<s32>;
using sizef32 = size<float>;

}  // namespace cacti