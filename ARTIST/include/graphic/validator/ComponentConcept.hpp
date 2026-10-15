#pragma once

#include <memory>
#include <type_traits>

namespace artist::graphic::validator
{
    // Borrowed contract: the component takes `C&` and returns void. This is
    // the tranche contract (Shader, Pass): a borrowed context must not extend
    // the lifetime of the owner.
    template <typename T, typename C>
    concept HasBorrowedOnMethod = requires(C &context) {
        {
            T::on(context)
        } -> std::same_as<void>;
    };

    // Shared contract (legacy call sites outside migrated tranches, e.g.
    // Uniform and Attribute components): `static void on(std::shared_ptr<C>)`.
    // Kept as the original concept so unrelated concepts remain untouched.
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
