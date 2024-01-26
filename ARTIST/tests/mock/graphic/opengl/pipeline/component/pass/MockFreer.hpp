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
        static std::shared_ptr<MockFreer<PROFILE>> instance()
        {
            static auto instance = std::make_shared<MockFreer<PROFILE>>();
            return instance;
        }

        static void reset()
        {
            ::testing::Mock::VerifyAndClearExpectations(instance().get());
        }

        static void on(std::shared_ptr<graphic::api::MockOpenGL::PassContext> openglContext)
        {
            instance()->mockOn(openglContext);
        }

        MOCK_METHOD(void, mockOn, (std::shared_ptr<graphic::api::MockOpenGL::PassContext>), ());
    };

}