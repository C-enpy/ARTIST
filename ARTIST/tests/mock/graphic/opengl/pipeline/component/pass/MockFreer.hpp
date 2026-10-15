#pragma once

#include <memory>
#include <gmock/gmock.h>
#include <graphic/MockApi.hpp>
#include <graphic/opengl/profile/Pass.hpp>

namespace artist::mock::graphic::opengl::pipeline::component::pass
{
    template <auto PROFILE>
    class MockFreer
    {
    public:
        static MockFreer<PROFILE> &instance()
        {
            static MockFreer<PROFILE> instance;
            return instance;
        }

        static void reset()
        {
            ::testing::Mock::VerifyAndClearExpectations(&instance());
        }

        static void on(typename graphic::api::MockOpenGL::PassContext &openglContext)
        {
            instance().mockOn(openglContext);
        }

        MOCK_METHOD(void, mockOn, (typename graphic::api::MockOpenGL::PassContext &), ());
    };

} // namespace artist::mock::graphic::opengl::pipeline::component::pass
