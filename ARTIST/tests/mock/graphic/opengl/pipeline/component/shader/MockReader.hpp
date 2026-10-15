// file: Reader.hpp

#pragma once

#include <memory>
#include <gmock/gmock.h>
#include <graphic/MockApi.hpp>
#include <graphic/opengl/profile/Shader.hpp>

namespace artist::mock::graphic::opengl::pipeline::component::shader
{
    template <auto PROFILE>
    class MockReader
    {
    public:
        static MockReader<PROFILE> &instance()
        {
            static MockReader<PROFILE> instance;
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

        MOCK_METHOD(void, mockOn, (typename graphic::api::MockOpenGL::ShaderContext &), ());
    };
} // namespace artist::mock::graphic::opengl::pipeline::component::shader
