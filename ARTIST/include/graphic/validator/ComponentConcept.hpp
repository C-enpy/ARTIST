#pragma once

#include <memory>
#include <type_traits>

namespace artist::graphic::validator
{
    // Borrowed contract: the component takes `C&` and returns void. This is
    // the tranche contract (Shader, Pass, Uniform, Attribute): a borrowed
    // context must not extend the lifetime of the owner.
    template <typename T, typename C>
    concept HasBorrowedOnMethod = requires(C &context) {
        {
            T::on(context)
        } -> std::same_as<void>;
    };

    // Shared contract: `static void on(std::shared_ptr<C>)`. No validator
    // requires it; contract tests use it to check that a component offers no
    // shared entry point.
    template <typename T, typename C>
    concept HasOnMethod = requires(std::shared_ptr<C> context) {
        {
            T::on(context)
        } -> std::same_as<void>;
    };

    template <typename C>
    concept HasComponent = requires {
        typename C;
    };
}
