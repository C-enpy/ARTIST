#include <concepts>
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
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
}
