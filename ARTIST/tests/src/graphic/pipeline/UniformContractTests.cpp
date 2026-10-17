#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>
#include <glm/glm.hpp>
#include <graphic/opengl/context/UniformContext.hpp>
#include <graphic/opengl/pipeline/component/uniform/Setter.hpp>
#include <graphic/opengl/validator/UniformConcept.hpp>
#include <graphic/validator/ComponentConcept.hpp>
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

    // The wrapper owns its context alone and only lends it.
    using Context = OpenGL::UniformContext;
    using Uniform = pipeline::Uniform<OpenGL>;
    static_assert(!std::is_copy_constructible_v<Uniform>);
    static_assert(!std::is_copy_assignable_v<Uniform>);
    static_assert(std::is_nothrow_move_constructible_v<Uniform>);
    static_assert(std::is_move_assignable_v<Uniform>);
    static_assert(std::is_constructible_v<Uniform, std::unique_ptr<Context>>);
    static_assert(!std::is_constructible_v<Uniform, std::shared_ptr<Context>>);
    static_assert(std::is_same_v<decltype(std::declval<const Uniform &>().getContext()), Context &>);
    // A missing or mismatched value is reported by throwing from the read.
    static_assert(!noexcept(std::declval<const Context &>().getValue<float>()));

    // Setter contract: a borrowed `on(C&)` returning void. Each rejected setter
    // differs from the accepted one by a single trait.
    template <typename T, typename C>
    concept AcceptsBorrowedContext = requires(C &context) { T::on(context); };

    struct BorrowedUniformSetter
    {
        template <typename C>
        static void on(C &) {}
    };
    struct SharedOnlyUniformSetter
    {
        template <typename C>
        static void on(std::shared_ptr<C>) {}
    };
    struct NonVoidUniformSetter
    {
        template <typename C>
        static int on(C &) { return 0; }
    };
    struct NoOnUniformSetter
    {
    };
    template <typename S>
    struct UniformContractAPI
    {
        struct UniformContext
        {
            template <typename> using Setter = S;
        };
    };
    template <typename S>
    using FakeContext = typename UniformContractAPI<S>::UniformContext;

    using artist::graphic::opengl::validator::OpenGLUniformFlow;
    static_assert(OpenGLUniformFlow<UniformContractAPI<BorrowedUniformSetter>>);
    static_assert(!OpenGLUniformFlow<UniformContractAPI<SharedOnlyUniformSetter>>);
    static_assert(!AcceptsBorrowedContext<SharedOnlyUniformSetter, FakeContext<SharedOnlyUniformSetter>>);
    static_assert(!OpenGLUniformFlow<UniformContractAPI<NonVoidUniformSetter>>);
    static_assert(AcceptsBorrowedContext<NonVoidUniformSetter, FakeContext<NonVoidUniformSetter>>);
    static_assert(!OpenGLUniformFlow<UniformContractAPI<NoOnUniformSetter>>);

    // Every OpenGL setter specialization offers the borrowed entry point only.
    using artist::graphic::opengl::pipeline::component::uniform::OpenGLUniformSetter;
    using artist::graphic::validator::HasBorrowedOnMethod;
    using artist::graphic::validator::HasOnMethod;
    template <typename... T>
    constexpr bool onlyBorrowedSetters = ((HasBorrowedOnMethod<OpenGLUniformSetter<T>, Context> && !HasOnMethod<OpenGLUniformSetter<T>, Context>) && ...);
    static_assert(onlyBorrowedSetters<GLfloat, GLint, GLuint, GLdouble,
                                      glm::vec2, glm::vec3, glm::vec4, glm::dvec2, glm::dvec3, glm::dvec4,
                                      glm::ivec2, glm::ivec3, glm::ivec4, glm::uvec2, glm::uvec3, glm::uvec4,
                                      glm::mat2, glm::mat3, glm::mat4, glm::mat2x3, glm::mat3x2, glm::mat2x4, glm::mat4x2, glm::mat3x4, glm::mat4x3,
                                      glm::dmat2, glm::dmat3, glm::dmat4, glm::dmat2x3, glm::dmat3x2, glm::dmat2x4, glm::dmat4x2, glm::dmat3x4, glm::dmat4x3>);
}
