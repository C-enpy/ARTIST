// file: Freer.hpp

#pragma once

#include <memory>
#include <gmock/gmock.h>
#include <graphic/MockApi.hpp>
#include <graphic/opengl/profile/Shader.hpp>

namespace artist::mock::graphic::opengl::pipeline::component::shader
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

        static void on(typename graphic::api::MockOpenGL::ShaderContext &openglContext)
        {
            instance().mockOn(openglContext);
        }

        MOCK_METHOD(void, mockOn, (typename graphic::api::MockOpenGL::ShaderContext & context), ());
    };
} // namespace artist::mock::graphic::opengl::pipeline::component::shader
