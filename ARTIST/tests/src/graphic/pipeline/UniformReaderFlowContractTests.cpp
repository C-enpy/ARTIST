#include <gtest/gtest.h>
#include <memory>

#include <graphic/opengl/profile/Pass.hpp>
#include <graphic/opengl/validator/PassConcept.hpp>

namespace
{
    struct BorrowedUniformFlowComponent
    {
        template <typename C> static void on(C &) {}
        template <typename C> static void on(C &, GLuint) {}
    };
    struct AbsentUniformReader {};
    struct SharedOnlyUniformReader
    {
        template <typename C> static void on(std::shared_ptr<C>) {}
    };
    struct NonVoidUniformReader
    {
        template <typename C> static int on(C &) { return 0; }
    };
    template <typename R>
    struct UniformReaderContractAPI
    {
        struct PassContext
        {
            template <auto> using Loader = BorrowedUniformFlowComponent;
            template <auto> using Freer = BorrowedUniformFlowComponent;
            template <auto> using ShaderAttacher = BorrowedUniformFlowComponent;
            template <auto> using AttributeReader = BorrowedUniformFlowComponent;
            template <auto> using User = BorrowedUniformFlowComponent;
            template <auto> using UniformReader = R;
        };
    };
    constexpr auto ClassicUniformFlow = artist::graphic::opengl::profile::Pass::Classic;
    template <typename R>
    concept ValidUniformReaderFlow = artist::graphic::opengl::validator::OpenGLPassFlow<UniformReaderContractAPI<R>, ClassicUniformFlow>;

    static_assert(ValidUniformReaderFlow<BorrowedUniformFlowComponent>);
    static_assert(!ValidUniformReaderFlow<AbsentUniformReader>, "F7 absence: UniformReader is required");
    static_assert(!ValidUniformReaderFlow<SharedOnlyUniformReader>, "F7 borrowed: shared-only UniformReader is invalid");
    static_assert(!ValidUniformReaderFlow<NonVoidUniformReader>, "F7 void: non-void UniformReader is invalid");
}

TEST(UniformReaderFlowContractTests, FlowRequiresBorrowedVoidUniformReader)
{
    EXPECT_TRUE((ValidUniformReaderFlow<BorrowedUniformFlowComponent>));
    EXPECT_FALSE((ValidUniformReaderFlow<AbsentUniformReader>));
    EXPECT_FALSE((ValidUniformReaderFlow<SharedOnlyUniformReader>));
    EXPECT_FALSE((ValidUniformReaderFlow<NonVoidUniformReader>));
}
