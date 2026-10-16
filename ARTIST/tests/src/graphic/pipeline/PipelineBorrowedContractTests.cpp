#include <gtest/gtest.h>
#include <memory>
#include <type_traits>

#include <graphic/Api.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <graphic/opengl/context/PipelineContext.hpp>
#include <graphic/opengl/pipeline/component/pipeline/Resetter.hpp>
#include <graphic/opengl/pipeline/component/pipeline/User.hpp>
#include <graphic/opengl/profile/Pipeline.hpp>
#include <graphic/opengl/validator/PipelineConcept.hpp>

namespace
{
    struct BorrowedPipelineComponent
    {
        template <typename C>
        static void on(C &) {}
    };
    struct SharedOnlyPipelineComponent
    {
        template <typename C>
        static void on(std::shared_ptr<C>) {}
    };
    struct NonVoidPipelineComponent
    {
        template <typename C>
        static int on(C &) { return 0; }
    };
    template <typename U, typename R>
    struct PipelineContractAPI
    {
        struct PipelineContext
        {
            template <auto> using User = U;
            template <auto> using Resetter = R;
        };
    };
    using BorrowedAPI = PipelineContractAPI<BorrowedPipelineComponent, BorrowedPipelineComponent>;
    using SharedUserAPI = PipelineContractAPI<SharedOnlyPipelineComponent, BorrowedPipelineComponent>;
    using SharedResetterAPI = PipelineContractAPI<BorrowedPipelineComponent, SharedOnlyPipelineComponent>;
    using NonVoidAPI = PipelineContractAPI<NonVoidPipelineComponent, BorrowedPipelineComponent>;
    constexpr auto Classic = artist::graphic::opengl::profile::Pipeline::Classic;
    static_assert(artist::graphic::opengl::validator::OpenGLPipelineFlow<BorrowedAPI, Classic>);
    static_assert(!artist::graphic::opengl::validator::OpenGLPipelineFlow<SharedUserAPI, Classic>);
    static_assert(!artist::graphic::opengl::validator::OpenGLPipelineFlow<SharedResetterAPI, Classic>);
    static_assert(!artist::graphic::opengl::validator::OpenGLPipelineFlow<NonVoidAPI, Classic>);
}

TEST(PipelineBorrowedContractTests, FlowRequiresBorrowedVoidComponents)
{
    using namespace artist::graphic::opengl::validator;
    EXPECT_TRUE((OpenGLPipelineFlow<BorrowedAPI, Classic>));
    EXPECT_FALSE((OpenGLPipelineFlow<SharedUserAPI, Classic>));
    EXPECT_FALSE((OpenGLPipelineFlow<SharedResetterAPI, Classic>));
    EXPECT_FALSE((OpenGLPipelineFlow<NonVoidAPI, Classic>));
}

TEST(PipelineBorrowedContractTests, RealComponentsOfferBorrowedEntryPoints)
{
    using C = artist::graphic::api::OpenGL::PipelineContext;
    using namespace artist::graphic::opengl::pipeline::component::pipeline;
    using artist::graphic::validator::HasBorrowedOnMethod;
    EXPECT_TRUE((HasBorrowedOnMethod<OpenGLPipelineUser<Classic>, C>));
    EXPECT_TRUE((HasBorrowedOnMethod<OpenGLPipelineResetter<Classic>, C>));
}
