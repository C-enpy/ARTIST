#include <concepts>
#include <graphic/opengl/context/UniformContext.hpp>
#include <graphic/pipeline/Uniform.hpp>

namespace
{
    namespace pipeline = artist::graphic::pipeline;
    using OpenGL = artist::graphic::api::OpenGL;

    template <typename API>
    concept HasUniform = requires { typename pipeline::Uniform<API>; };

    struct RejectingValidator
    {
        template <typename API>
        static constexpr bool validateUniform() { return false; }
    };

    struct RejectedApi
    {
        using Validator = RejectingValidator;
    };

    template <typename API>
    struct DerivedUniform : pipeline::Uniform<API> {};

    static_assert(HasUniform<OpenGL>);
    static_assert(!HasUniform<RejectedApi>);
    static_assert(!HasUniform<int>);
    static_assert(std::derived_from<DerivedUniform<OpenGL>, pipeline::Uniform<OpenGL>>);
}
