#pragma once

#include <gmock/gmock.h>
#include <graphic/pipeline/MockShader.hpp>
#include <graphic/opengl/context/MockPassContext.hpp>
#include <graphic/opengl/pipeline/component/pass/MockShaderAttacher.hpp>
#include <graphic/opengl/pipeline/component/pass/MockUniformReader.hpp>
#include <graphic/opengl/pipeline/component/pass/MockAttributeReader.hpp>
#include <graphic/opengl/pipeline/component/pass/MockUser.hpp>
#include <graphic/opengl/pipeline/component/pass/MockFreer.hpp>
#include <graphic/opengl/pipeline/component/pass/MockLoader.hpp>
#include <graphic/pipeline/Pass.hpp>

namespace artist::mock::graphic::pipeline::opengl
{
    namespace pipeline = artist::graphic::pipeline;

    template <typename API>
    class MockPass : public pipeline::IPass<API>
    {
    public:
        MockPass() : pipeline::IPass<API>(std::vector<std::unique_ptr<pipeline::IShader<API>>>())
        {
        }

        MOCK_METHOD(void, use, (), (override));
        MOCK_METHOD(void, load, (), (override));
        MOCK_METHOD(void, free, (), (override));

        MOCK_METHOD((const std::unordered_map<std::string, std::shared_ptr<pipeline::Uniform<API>>, collection_utils::StringHash, collection_utils::StringEqual> &), getUniforms, (), (const, override));
        MOCK_METHOD(pipeline::IShader<API> &, getShader, (const int &), (const, override));
        MOCK_METHOD(int, getShadersCount, (), (const, override));

    protected:
        MOCK_METHOD(void, readUniforms, (typename API::PassContext &), (override));
        MOCK_METHOD(void, readAttributes, (typename API::PassContext &), (override));
        MOCK_METHOD(void, free, (typename API::PassContext &), (override));
        MOCK_METHOD(void, use, (typename API::PassContext &), (override));
        MOCK_METHOD(void, load, (typename API::PassContext &), (override));
    };
} // namespace artist::mock::graphic::pipeline::opengl
