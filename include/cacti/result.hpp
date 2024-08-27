#pragma once

#include <memory>
#include <type_traits>

#include "cacti/stdlib.hpp"

namespace cacti {

template <typename return_type, typename error_type>
struct result final {
    static_assert(!std::is_same_v<return_type, error_type>,
                  "Return and error types cannot be the same type");

   public:
    constexpr result(return_type success)
        : _failed(false), _success(std::move(success)) {}

    constexpr result(error_type failure)
        : _failed(true), _failure(std::move(failure)) {}

    [[nodiscard]] constexpr bool failed() const noexcept { return _failed; }

    [[nodiscard]] constexpr const return_type& value() const noexcept {
        return _success;
    }

    [[nodiscard]] constexpr const error_type& error() const noexcept {
        return _failure;
    }

   private:
    return_type _success{};
    error_type _failure{};
    bool _failed;
};

}  // namespace cacti