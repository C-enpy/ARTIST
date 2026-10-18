#include <concepts>
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <array>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/ext/vector_float1.hpp>
#include <glm/ext/vector_int1.hpp>
#include <glm/ext/vector_uint1.hpp>
#include <graphic/Api.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <graphic/opengl/context/AttributeContext.hpp>
#include <graphic/opengl/profile/Attribute.hpp>
#include <graphic/opengl/pipeline/component/attribute/Binder.hpp>
#include <graphic/opengl/pipeline/component/attribute/Setter.hpp>
#include <graphic/opengl/pipeline/component/attribute/Unbinder.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/pipeline/Attribute.hpp>

namespace
{
    namespace pipeline = artist::graphic::pipeline;
    namespace attribute = artist::graphic::opengl::pipeline::component::attribute;
    using OpenGL = artist::graphic::api::OpenGL;
    using artist::graphic::opengl::profile::Attribute::Classic;

    template <typename T, typename Arg>
    concept AcceptsOn = requires(Arg &&argument) { T::on(std::forward<Arg>(argument)); };

    // The wrapper owns its context alone and only lends it.
    using Context = OpenGL::AttributeContext;
    using Attribute = pipeline::Attribute<OpenGL, Classic>;
    static_assert(!std::is_copy_constructible_v<Attribute>);
    static_assert(!std::is_copy_assignable_v<Attribute>);
    static_assert(std::is_nothrow_move_constructible_v<Attribute>);
    static_assert(std::is_move_assignable_v<Attribute>);
    static_assert(std::is_constructible_v<Attribute, std::unique_ptr<Context>>);
    static_assert(!std::is_constructible_v<Attribute, std::shared_ptr<Context>>);
    static_assert(std::is_same_v<decltype(std::declval<const Attribute &>().getContext()), Context &>);
    // Passes destroy their attributes through the interface.
    static_assert(std::has_virtual_destructor_v<pipeline::IAttribute<OpenGL>>);

    // The pass owns its attributes alone and only lends them.
    using PassContext = artist::graphic::opengl::context::OpenGLPassContext;
    using OwnedAttribute = std::unique_ptr<pipeline::IAttribute<OpenGL>>;
    template <typename P>
    concept AddsAttribute = requires(PassContext &context, P attribute) { context.addAttribute("a", std::move(attribute)); };
    static_assert(AddsAttribute<OwnedAttribute>);
    static_assert(!AddsAttribute<std::shared_ptr<pipeline::IAttribute<OpenGL>>>);
    static_assert(std::is_same_v<decltype(std::declval<const PassContext &>().getAttribute("a")), pipeline::IAttribute<OpenGL> *>);
    static_assert(std::is_same_v<std::remove_cvref_t<decltype(std::declval<const PassContext &>().getAttributes())>::mapped_type, OwnedAttribute>);

    // OpenGL components borrow the context: a reference is accepted, a null
    // pointer or a shared pointer is not. Each rejected call differs from the
    // accepted one by its argument type only.
    using Setter = attribute::OpenGLSetter<float>;
    using Binder = attribute::OpenGLBinder<Classic>;
    using Unbinder = attribute::OpenGLUnbinder<Classic>;
    static_assert(AcceptsOn<Setter, Context &>);
    static_assert(!AcceptsOn<Setter, std::nullptr_t>);
    static_assert(!AcceptsOn<Setter, std::shared_ptr<Context>>);
    static_assert(AcceptsOn<Binder, Context &>);
    static_assert(!AcceptsOn<Binder, std::nullptr_t>);
    static_assert(!AcceptsOn<Binder, std::shared_ptr<Context>>);
    static_assert(AcceptsOn<Unbinder, Context &>);
    static_assert(!AcceptsOn<Unbinder, std::nullptr_t>);
    static_assert(!AcceptsOn<Unbinder, std::shared_ptr<Context>>);

    using artist::graphic::opengl::validator::OpenGLAttributeFlow;
    static_assert(OpenGLAttributeFlow<OpenGL, Classic>);

    // Flow contract: each component takes `C&` and returns void. Each rejected
    // API differs from the accepted one by a single component.
    struct BorrowedComponent
    {
        template <typename C>
        static void on(C &) {}
    };
    struct SharedOnlyComponent
    {
        template <typename C>
        static void on(std::shared_ptr<C>) {}
    };
    struct NonVoidComponent
    {
        template <typename C>
        static int on(C &) { return 0; }
    };
    struct NoOnComponent
    {
    };
    template <typename B, typename U, typename S>
    struct AttributeContractAPI
    {
        struct AttributeContext
        {
            template <auto> using Binder = B;
            template <auto> using Unbinder = U;
            template <typename> using Setter = S;
        };
    };
    using Borrowed = BorrowedComponent;
    template <typename S>
    using FakeContext = typename AttributeContractAPI<Borrowed, Borrowed, S>::AttributeContext;

    static_assert(OpenGLAttributeFlow<AttributeContractAPI<Borrowed, Borrowed, Borrowed>, Classic>);
    static_assert(!OpenGLAttributeFlow<AttributeContractAPI<SharedOnlyComponent, Borrowed, Borrowed>, Classic>);
    static_assert(!OpenGLAttributeFlow<AttributeContractAPI<Borrowed, SharedOnlyComponent, Borrowed>, Classic>);
    static_assert(!OpenGLAttributeFlow<AttributeContractAPI<Borrowed, Borrowed, SharedOnlyComponent>, Classic>);
    static_assert(!AcceptsOn<SharedOnlyComponent, FakeContext<SharedOnlyComponent> &>);
    static_assert(!OpenGLAttributeFlow<AttributeContractAPI<Borrowed, Borrowed, NonVoidComponent>, Classic>);
    static_assert(AcceptsOn<NonVoidComponent, FakeContext<NonVoidComponent> &>);
    static_assert(!OpenGLAttributeFlow<AttributeContractAPI<Borrowed, Borrowed, NoOnComponent>, Classic>);

    // The gate refuses an API without a Setter, so no Attribute of that API can
    // exist and IAttribute::set never reaches UNSUPPORTED_TYPE at run time.
    struct NoSetterAPI
    {
        struct AttributeContext
        {
            template <auto> using Binder = Borrowed;
            template <auto> using Unbinder = Borrowed;
        };
    };
    static_assert(!OpenGLAttributeFlow<NoSetterAPI, Classic>);

    template <typename T, std::size_t N> consteval bool packedLayout()
    {
        if (sizeof(T) != N * 4 || alignof(T) != 4 || !std::is_standard_layout_v<T>) return false;
        if constexpr (N >= 2) { if (offsetof(T, x) != 0 || offsetof(T, y) != 4) return false; }
        if constexpr (N >= 3) { if (offsetof(T, z) != 8) return false; }
        if constexpr (N >= 4) { if (offsetof(T, w) != 12) return false; }
        return true;
    }
    template <typename T, std::size_t N> consteval bool supportedPacked()
    {
        return packedLayout<T, N>() && AcceptsOn<attribute::OpenGLSetter<T>, Context&>;
    }
    static_assert(supportedPacked<float, 1>() && supportedPacked<glm::vec2, 2>() && supportedPacked<glm::vec3, 3>() && supportedPacked<glm::vec4, 4>());
    static_assert(supportedPacked<std::int32_t, 1>() && supportedPacked<glm::ivec2, 2>() && supportedPacked<glm::ivec3, 3>() && supportedPacked<glm::ivec4, 4>());
    static_assert(supportedPacked<std::uint32_t, 1>() && supportedPacked<glm::uvec2, 2>() && supportedPacked<glm::uvec3, 3>() && supportedPacked<glm::uvec4, 4>());
    static_assert(std::is_same_v<float, GLfloat> && std::is_same_v<std::int32_t, GLint> && std::is_same_v<std::int32_t, glm::int32> && std::is_same_v<std::int32_t, int>);
    static_assert(std::is_same_v<std::uint32_t, GLuint> && std::is_same_v<std::uint32_t, glm::uint32> && std::is_same_v<std::uint32_t, unsigned>);
    static_assert(std::is_same_v<glm::vec2, glm::highp_vec2> && std::is_same_v<glm::vec3, glm::highp_vec3> && std::is_same_v<glm::vec4, glm::highp_vec4>);
    static_assert(std::is_same_v<glm::ivec2, glm::highp_ivec2> && std::is_same_v<glm::ivec3, glm::highp_ivec3> && std::is_same_v<glm::ivec4, glm::highp_ivec4>);
    static_assert(std::is_same_v<glm::uvec2, glm::highp_uvec2> && std::is_same_v<glm::uvec3, glm::highp_uvec3> && std::is_same_v<glm::uvec4, glm::highp_uvec4>);
    static_assert(std::is_same_v<glm::ivec2, glm::i32vec2> && std::is_same_v<glm::ivec3, glm::i32vec3> && std::is_same_v<glm::ivec4, glm::i32vec4>);
    static_assert(std::is_same_v<glm::uvec2, glm::u32vec2> && std::is_same_v<glm::uvec3, glm::u32vec3> && std::is_same_v<glm::uvec4, glm::u32vec4>);
    struct AttributePackedLookalike { float x, y, z; };
    template <typename... Ts> constexpr bool rejectedBySetter = (!AcceptsOn<attribute::OpenGLSetter<Ts>, Context&> && ...);
    static_assert(rejectedBySetter<glm::mediump_vec2, glm::mediump_vec3, glm::mediump_vec4, glm::lowp_vec3,
        glm::mediump_ivec3, glm::lowp_ivec3, glm::mediump_uvec3, glm::lowp_uvec3>);
    static_assert(rejectedBySetter<glm::vec1, glm::ivec1, glm::uvec1, std::int8_t, std::int16_t, std::uint8_t, std::uint16_t>);
    static_assert(rejectedBySetter<double, glm::dvec3, glm::mat4, float[3], std::array<float, 3>, std::vector<float>, float*, AttributePackedLookalike, bool, const float>);
#if GLM_CONFIG_ALIGNED_GENTYPES == GLM_ENABLE
    static_assert(rejectedBySetter<glm::vec<3, float, glm::aligned_highp>>);
#endif
    static_assert(requires(Attribute& a) { a.set(std::shared_ptr<glm::mat4>{}); });
}
