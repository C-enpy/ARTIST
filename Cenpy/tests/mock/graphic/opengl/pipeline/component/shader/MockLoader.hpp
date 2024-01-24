// file: Loader.hpp

#pragma once

#include <memory>
#include <gmock/gmock.h>
#include <graphic/MockApi.hpp>
#include <graphic/opengl/profile/Shader.hpp>

namespace cenpy::mock::graphic::opengl::pipeline::component::shader
{
    template <auto PROFILE>
    class MockLoader
    {
    public:
        static std::shared_ptr<MockLoader<PROFILE>> instance()
        {
            static auto instance = std::make_shared<MockLoader<PROFILE>>();
            return instance;
        }

        static void reset()
        {
            ::testing::Mock::VerifyAndClearExpectations(instance().get());
        }

        static void on(std::shared_ptr<graphic::api::MockOpenGL::ShaderContext> openglContext)
        {
            instance()->mockOn(openglContext);
        }

        MOCK_METHOD(void, mockOn, (std::shared_ptr<graphic::api::MockOpenGL::ShaderContext>), ());
    };

}