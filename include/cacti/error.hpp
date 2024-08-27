#pragma once

#include <source_location>

#include "cacti/stdlib.hpp"

namespace cacti {

struct error {
   public:
    error() = default;

    error(const string& what, std::source_location where)
        : _what(what), _where(where) {}

    [[nodiscard]] const string& what() const noexcept { return _what; }
    [[nodiscard]] const std::source_location& where() const noexcept {
        return _where;
    }

   private:
    string _what{};
    std::source_location _where{};
};

#define MAKE_ERROR_HERE(what) \
    cacti::error((what), std::source_location::current())

}  // namespace cacti