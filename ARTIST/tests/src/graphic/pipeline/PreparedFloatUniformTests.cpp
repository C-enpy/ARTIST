#ifdef __mock_gl__
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstring>
#include <memory>
#include <string>
#include <opengl/glFunctionMock.hpp>
#include <graphic/opengl/pipeline/component/pass/UniformReader.hpp>
#include <graphic/opengl/pipeline/component/pass/Freer.hpp>
#include <graphic/opengl/pipeline/component/pass/Loader.hpp>
#include <graphic/opengl/pipeline/component/pass/ShaderAttacher.hpp>
#include <graphic/opengl/pipeline/component/pass/AttributeReader.hpp>
#include <graphic/opengl/pipeline/component/pass/User.hpp>
#include <graphic/opengl/pipeline/component/attribute/Binder.hpp>
#include <graphic/opengl/pipeline/component/attribute/Setter.hpp>
#include <graphic/opengl/pipeline/component/attribute/Unbinder.hpp>
#include <graphic/pipeline/Pass.hpp>
#include <optional>
#include <graphic/opengl/pipeline/PreparedFloatUniform.hpp>
#include <utility>
#include <graphic/pipeline/Shader.hpp>
#include <graphic/opengl/profile/Shader.hpp>
#include <graphic/opengl/pipeline/component/shader/Freer.hpp>
#include <graphic/opengl/pipeline/component/shader/Loader.hpp>
#include <graphic/opengl/pipeline/component/shader/Reader.hpp>
#include <graphic/opengl/validator/Validator.hpp>

namespace
{
    using API = artist::graphic::api::OpenGL;
    using Context = API::PassContext;
    using Mock = artist::mock::opengl::glFunctionMock;
    using Reader = Context::UniformReader<artist::graphic::opengl::profile::Pass::Classic>;
    using Error = artist::common::exception::TraceableException<std::runtime_error>;
    template <typename T>
    concept CanPrepareFloat = requires(T &pass)
    {
        artist::graphic::opengl::pipeline::prepareFloatUniform(pass, std::string("scalar"));
    };

    auto prepare(Context &context, const std::string &name)
    {
        return artist::graphic::opengl::pipeline::prepareFloatUniform(context, name);
    }

    class PreparedFloatUniformTest : public ::testing::Test
    {
    protected:
        Mock &mock = *Mock::instance();
        Context context;
        void SetUp() override
        {
            mock.resetContractState();
            mock.configureCurrentContext(reinterpret_cast<GLFWwindow *>(this));
            mock.configureCurrentProgram(7);
            context.setPassID(7);
            EXPECT_CALL(mock, glGetProgramiv_mock(7, GL_LINK_STATUS, ::testing::_))
                .Times(::testing::AnyNumber())
                .WillRepeatedly(::testing::SetArgPointee<2>(GL_TRUE));
        }
        void TearDown() override
        {
            Mock::reset();
            mock.resetContractState();
        }
        void reflect(const std::string &name, GLenum type = GL_FLOAT, GLint size = 1, Context *target = nullptr)
        {
            EXPECT_CALL(mock, glGetProgramiv_mock(7, GL_ACTIVE_UNIFORMS, ::testing::_))
                .WillOnce(::testing::SetArgPointee<2>(1));
            EXPECT_CALL(mock, glGetProgramiv_mock(7, GL_ACTIVE_UNIFORM_MAX_LENGTH, ::testing::_))
                .WillOnce(::testing::SetArgPointee<2>(static_cast<GLint>(name.size() + 1)));
            EXPECT_CALL(mock, glGetActiveUniform_mock(7, 0, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
                .WillOnce(::testing::Invoke([name, type, size](GLuint, GLuint, GLsizei capacity,
                    GLsizei *length, GLint *count, GLenum *kind, GLchar *buffer) {
                    ASSERT_EQ(capacity, name.size() + 1);
                    std::memcpy(buffer, name.c_str(), name.size() + 1);
                    *length = static_cast<GLsizei>(name.size());
                    *count = size;
                    *kind = type;
                }));
            EXPECT_CALL(mock, glGetUniformLocation_mock(7, ::testing::StrEq(name)))
                .WillOnce(::testing::Return(3));
            Reader::on(target ? *target : context);
        }
    };

    TEST_F(PreparedFloatUniformTest, ApiExistsAndCannotBeForgedFromNativeNames)
    {
        using Access = artist::graphic::opengl::pipeline::PreparedFloatUniform;
        static_assert(!std::is_default_constructible_v<Access>);
        static_assert(!std::is_constructible_v<Access, GLuint, GLint>);
        static_assert(requires(Access &access) { access.set(float{}); });
        static_assert(CanPrepareFloat<Context>);
        static_assert(!CanPrepareFloat<int>);
        static_assert(!CanPrepareFloat<const Context>);
    }

    TEST_F(PreparedFloatUniformTest, PassMovesFollowUniqueContextAndRetireLoadedDestinationAndDestruction)
    {
        using Pass = artist::graphic::pipeline::Pass<API, artist::graphic::opengl::profile::Pass::Classic>;
        using Access = artist::graphic::opengl::pipeline::PreparedFloatUniform;
        static_assert(!std::copy_constructible<Context> && !std::move_constructible<Context>);
        static_assert(!std::copy_constructible<API::UniformContext>);
        std::optional<Access> survivor;
        std::shared_ptr<const artist::graphic::context::detail::PreparedUniformState> heldWitness;
        EXPECT_CALL(mock, glDeleteProgram_mock(7)).Times(2);
        {
            auto sourceContext = std::make_unique<Context>();
            sourceContext->setPassID(7);
            reflect("scalar", GL_FLOAT, 1, sourceContext.get());
            Pass source(std::move(sourceContext));
            auto access = artist::graphic::opengl::pipeline::prepareFloatUniform(source, "scalar");
            Pass moved(std::move(source));
            EXPECT_THROW(source.getContext(), Error);
            access.set(.25f);
            auto destinationContext = std::make_unique<Context>();
            destinationContext->setPassID(7);
            reflect("scalar", GL_FLOAT, 1, destinationContext.get());
            Pass destination(std::move(destinationContext));
            auto previous = artist::graphic::opengl::pipeline::prepareFloatUniform(destination, "scalar");
            destination = std::move(moved);
            EXPECT_THROW(previous.set(.5f), Error);
            access.set(.5f);
            destination = std::move(destination);
            access.set(.75f);
            survivor = access;
            heldWitness = destination.getContext().preparedUniformState().lock();
        }
        const auto queries = mock.contextQueryCount();
        EXPECT_THROW(survivor->set(1), Error);
        EXPECT_EQ(mock.contextQueryCount(), queries);
    }

    TEST_F(PreparedFloatUniformTest, ScalarWritesTypedArgumentAndHistoricalCpuMirror)
    {
        reflect("scalar");
        auto access = prepare(context, "scalar");
        access.set(.5f);
        EXPECT_EQ(context.getUniform("scalar")->get<GLfloat>(), .5f);
        ASSERT_EQ(mock.uniformCalls().size(), 1u);
        EXPECT_EQ(mock.uniformCalls().front().location, 3);
        GLfloat payload = 0;
        ASSERT_EQ(mock.uniformCalls().front().payload.size(), sizeof(payload));
        std::memcpy(&payload, mock.uniformCalls().front().payload.data(), sizeof(payload));
        EXPECT_EQ(payload, .5f);
    }

    TEST_F(PreparedFloatUniformTest, ArrayOfOneRejectedBeforeCpuMutationOrUpload)
    {
        reflect("array[0]");
        auto *uniform = context.getUniform("array[0]");
        uniform->getContext().setValue(GLint(91));
        EXPECT_THROW({ auto access = prepare(context, "array[0]"); access.set(.5f); }, Error);
        EXPECT_TRUE(mock.uniformCalls().empty());
        EXPECT_EQ(uniform->get<GLint>(), 91);
    }

    TEST_F(PreparedFloatUniformTest, BooleanCompatibilityDoesNotLeakIntoStrictFloatAccess)
    {
        reflect("boolean", GL_BOOL);
        EXPECT_THROW({ auto access = prepare(context, "boolean"); access.set(.5f); }, Error);
        EXPECT_TRUE(mock.uniformCalls().empty());
    }

    TEST_F(PreparedFloatUniformTest, OtherTypesAndLargerArraysAreRejectedButLegacyBoolStillWorks)
    {
        for (GLenum type : {GL_INT, GL_UNSIGNED_INT, GL_DOUBLE, GL_FLOAT_VEC2, GL_FLOAT_MAT4})
        {
            reflect("scalar", type);
            EXPECT_THROW(prepare(context, "scalar"), Error);
        }
        reflect("array[0]", GL_FLOAT, 2);
        EXPECT_THROW(prepare(context, "array[0]"), Error);
        reflect("boolean", GL_BOOL);
        context.getUniform("boolean")->set(.25f);
        EXPECT_EQ(context.getUniform("boolean")->get<GLfloat>(), .25f);
        EXPECT_EQ(mock.uniformCalls().size(), 1u);
    }

    TEST_F(PreparedFloatUniformTest, PreparationRejectsMissingAndOtherContextBeforeNativeQueriesAndRecovers)
    {
        using Access = artist::graphic::opengl::pipeline::PreparedFloatUniform;
        reflect("scalar");
        auto *uniform = context.getUniform("scalar");
        ASSERT_NE(uniform, nullptr);
        uniform->getContext().setValue(GLfloat(.25f));
        const auto witness = context.preparedUniformState().lock();
        ASSERT_NE(witness, nullptr);
        const auto generation = witness->generation();

        GLFWwindow *invalidContexts[] = {
            nullptr, reinterpret_cast<GLFWwindow *>(&context)
        };
        for (int mode = 0; mode != 2; ++mode)
        {
            SCOPED_TRACE(mode);
            mock.configureCurrentContext(invalidContexts[mode]);
            // Keep the successful link response so removing the context guard
            // cannot be hidden by a different native link-status rejection.
            const auto calls = mock.glCalls().size();
            std::optional<Access> rejected;
            try
            {
                rejected.emplace(prepare(context, "scalar"));
                ADD_FAILURE() << "Preparation returned an access under an invalid context";
            }
            catch (const Error &error)
            {
                EXPECT_STREQ(error.what(), mode == 0
                    ? "ERROR::UNIFORM::SET::NO_CURRENT_CONTEXT"
                    : "ERROR::UNIFORM::SET::WRONG_CONTEXT");
            }
            EXPECT_FALSE(rejected.has_value());
            EXPECT_EQ(mock.glCalls().size(), calls);
            EXPECT_TRUE(mock.uniformCalls().empty());
            EXPECT_EQ(context.getUniform("scalar"), uniform);
            EXPECT_EQ(uniform->get<GLfloat>(), .25f);
            EXPECT_EQ(witness->generation(), generation);
            EXPECT_TRUE(witness->accepts(generation));
        }

        mock.configureCurrentContext(reinterpret_cast<GLFWwindow *>(this));
        auto access = prepare(context, "scalar");
        access.set(.75f);
        EXPECT_EQ(uniform->get<GLfloat>(), .75f);
        ASSERT_EQ(mock.uniformCalls().size(), 1u);
        const auto &call = mock.uniformCalls().front();
        EXPECT_EQ(call.location, 3);
        GLfloat payload = 0;
        ASSERT_EQ(call.payload.size(), sizeof(payload));
        std::memcpy(&payload, call.payload.data(), sizeof(payload));
        EXPECT_EQ(payload, .75f);
    }

    TEST_F(PreparedFloatUniformTest, PartialDetachFailureRetiresBeforeFirstEffectAndRejectsRepeatedSets)
    {
        using Shader = artist::graphic::pipeline::Shader<
            API, artist::graphic::opengl::profile::Shader::Classic>;
        using Pass = artist::graphic::pipeline::Pass<
            API, artist::graphic::opengl::profile::Pass::Classic>;
        auto ownerContext = std::make_unique<Context>();
        ownerContext->setPassID(7);
        auto first = std::make_unique<Shader>(
            "", artist::graphic::context::ShaderType::VERTEX);
        auto second = std::make_unique<Shader>(
            "", artist::graphic::context::ShaderType::VERTEX);
        first->getContext().setShaderID(61);
        second->getContext().setShaderID(62);
        auto *secondShader = second.get();
        ownerContext->addShader(std::move(first));
        ownerContext->addShader(std::move(second));
        Pass owner(std::move(ownerContext));
        auto &passContext = owner.getContext();
        reflect("scalar", GL_FLOAT, 1, &passContext);
        auto access = prepare(passContext, "scalar");
        auto *uniform = passContext.getUniform("scalar");
        ASSERT_NE(uniform, nullptr);
        access.set(.25f);
        ASSERT_EQ(mock.uniformCalls().size(), 1u);
        EXPECT_EQ(uniform->get<GLfloat>(), .25f);
        const auto witness = passContext.preparedUniformState().lock();
        ASSERT_NE(witness, nullptr);
        const auto generation = witness->generation();
        ASSERT_TRUE(witness->accepts(generation));

        Shader parked(std::move(*secondShader));
        bool firstDetached = false;
        EXPECT_CALL(mock, glDetachShader_mock(7, 61))
            .WillOnce(::testing::Invoke([&](GLuint, GLuint) {
                firstDetached = true;
                // Observe revocation at the first native effect, without
                // re-entering the prepared access.
                EXPECT_FALSE(witness->accepts(generation));
            }));
        EXPECT_CALL(mock, glDetachShader_mock(7, 62)).Times(0);
        EXPECT_CALL(mock, glDeleteProgram_mock(::testing::_)).Times(0);
        EXPECT_CALL(mock, glDeleteShader_mock(::testing::_)).Times(0);
        const auto callsBeforeFree = mock.glCalls().size();
        const auto queriesBeforeFree = mock.contextQueryCount();
        try
        {
            Context::Freer<artist::graphic::opengl::profile::Pass::Classic>::on(passContext);
            ADD_FAILURE() << "Expected rejection of the moved-from second shader";
        }
        catch (const Error &error)
        {
            EXPECT_STREQ(error.what(), "ERROR::SHADER::FREED_CONTEXT");
        }
        // Restore the owned wrapper before assertions and owner destruction.
        *secondShader = std::move(parked);

        EXPECT_TRUE(firstDetached);
        EXPECT_EQ(mock.glCalls().size(), callsBeforeFree + 1);
        EXPECT_EQ(mock.contextQueryCount(), queriesBeforeFree);
        EXPECT_EQ(passContext.getPassID(), 7u);
        EXPECT_EQ(secondShader->getContext().getShaderID(), 62u);
        EXPECT_FALSE(witness->accepts(generation));
        EXPECT_EQ(passContext.getUniform("scalar"), uniform);
        const auto callsAfterFailure = mock.glCalls().size();
        const auto queriesAfterFailure = mock.contextQueryCount();
        for (int attempt = 0; attempt != 2; ++attempt)
        {
            SCOPED_TRACE(attempt);
            try
            {
                access.set(.5f);
                ADD_FAILURE() << "Expected partial-retirement revocation";
            }
            catch (const Error &error)
            {
                EXPECT_STREQ(error.what(),
                    "ERROR::UNIFORM::SET::STALE_PREPARED_ACCESS");
            }
            EXPECT_EQ(mock.glCalls().size(), callsAfterFailure);
            EXPECT_EQ(mock.contextQueryCount(), queriesAfterFailure);
            EXPECT_EQ(mock.uniformCalls().size(), 1u);
            EXPECT_EQ(uniform->get<GLfloat>(), .25f);
        }

        EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(&mock));
        {
            ::testing::InSequence sequence;
            EXPECT_CALL(mock, glDetachShader_mock(7, 61)).Times(1);
            EXPECT_CALL(mock, glDetachShader_mock(7, 62)).Times(1);
            EXPECT_CALL(mock, glDeleteProgram_mock(7)).Times(1);
        }
        EXPECT_CALL(mock, glDeleteShader_mock(61)).Times(1);
        EXPECT_CALL(mock, glDeleteShader_mock(62)).Times(1);
    }

    TEST_F(PreparedFloatUniformTest, NativeRetirementFailureNeverRehabilitatesOldAccess)
    {
        reflect("scalar");
        auto old = prepare(context, "scalar");
        EXPECT_CALL(mock, glDeleteProgram_mock(7))
            .WillOnce(::testing::Throw(std::runtime_error("native cleanup failed")));
        EXPECT_THROW((Context::Freer<artist::graphic::opengl::profile::Pass::Classic>::on(context)), std::runtime_error);
        EXPECT_EQ(context.getPassID(), 7u);
        EXPECT_THROW(old.set(.5f), Error);
        EXPECT_THROW(prepare(context, "scalar"), Error);
        EXPECT_TRUE(mock.uniformCalls().empty());
    }

    TEST_F(PreparedFloatUniformTest, RejectsAbsentInvalidNamesAndFabricatedMetadata)
    {
        reflect("scalar");
        EXPECT_THROW(prepare(context, "absent"), Error);
        EXPECT_THROW(prepare(context, ""), Error);
        EXPECT_THROW(prepare(context, std::string("scalar\0suffix", 13)), Error);
        auto fabricated = std::make_unique<artist::graphic::pipeline::Uniform<API>>();
        fabricated->getContext().setUniformID(3);
        fabricated->getContext().setProgramID(7);
        fabricated->getContext().setGLType(GL_FLOAT);
        context.addUniform("scalar", std::move(fabricated));
        EXPECT_THROW(prepare(context, "scalar"), Error);
        EXPECT_TRUE(mock.uniformCalls().empty());
    }

    TEST_F(PreparedFloatUniformTest, ReflectedMetadataCannotBeSmuggledFromAnotherOwner)
    {
        reflect("scalar");
        auto old = prepare(context, "scalar");
        Context other;
        other.setPassID(7); // numeric equality does not attest ownership
        reflect("scalar", GL_FLOAT, 1, &other);
        *context.getUniform("scalar") = std::move(*other.getUniform("scalar"));
        EXPECT_THROW(old.set(.5f), Error);
        try { (void)prepare(context, "scalar"); FAIL(); }
        catch (const Error &error)
        {
            EXPECT_THAT(error.what(), ::testing::HasSubstr("UNTRUSTED_METADATA"));
        }
        EXPECT_TRUE(mock.uniformCalls().empty());
    }

    TEST_F(PreparedFloatUniformTest, AmbiguousReflectedLocationsAreNotPrepared)
    {
        EXPECT_CALL(mock, glGetProgramiv_mock(7, GL_ACTIVE_UNIFORMS, ::testing::_))
            .WillOnce(::testing::SetArgPointee<2>(2));
        EXPECT_CALL(mock, glGetProgramiv_mock(7, GL_ACTIVE_UNIFORM_MAX_LENGTH, ::testing::_))
            .WillOnce(::testing::SetArgPointee<2>(6));
        for (GLuint index = 0; index != 2; ++index)
        {
            EXPECT_CALL(mock, glGetActiveUniform_mock(7, index, 6, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
                .WillOnce(::testing::Invoke([index](GLuint, GLuint, GLsizei, GLsizei *length,
                    GLint *size, GLenum *type, GLchar *buffer) {
                    std::memcpy(buffer, index == 0 ? "first" : "other", 6);
                    *length = 5; *size = 1; *type = GL_FLOAT;
                }));
        }
        EXPECT_CALL(mock, glGetUniformLocation_mock(7, ::testing::_))
            .Times(2).WillRepeatedly(::testing::Return(3));
        Reader::on(context);
        try { (void)prepare(context, "first"); FAIL(); }
        catch (const Error &error)
        {
            EXPECT_THAT(error.what(), ::testing::HasSubstr("AMBIGUOUS_LOCATION"));
        }
        EXPECT_TRUE(mock.uniformCalls().empty());
    }

    TEST_F(PreparedFloatUniformTest, ProgramLinkAndNewIncarnationGuardsHaveDeterministicDiagnostics)
    {
        reflect("scalar");
        EXPECT_CALL(mock, glGetProgramiv_mock(7, GL_LINK_STATUS, ::testing::_))
            .WillOnce(::testing::SetArgPointee<2>(GL_FALSE));
        try { (void)prepare(context, "scalar"); FAIL(); }
        catch (const Error &error)
        {
            EXPECT_THAT(error.what(), ::testing::HasSubstr("PROGRAM_NOT_LINKED"));
        }
        context.setPassID(7);
        try { (void)prepare(context, "scalar"); FAIL(); }
        catch (const Error &error)
        {
            EXPECT_THAT(error.what(), ::testing::HasSubstr("REFLECTION_NOT_CURRENT"));
        }
    }

    TEST_F(PreparedFloatUniformTest, MetadataMutationEvenToSameValueRevokesAndCannotReattest)
    {
        for (int mutation = 0; mutation != 4; ++mutation)
        {
            reflect("scalar");
            auto access = prepare(context, "scalar");
            auto &uniform = context.getUniform("scalar")->getContext();
            uniform.setValue(GLint(91));
            switch (mutation)
            {
            case 0: uniform.setUniformID(3); break;
            case 1: uniform.setProgramID(7); break;
            case 2: uniform.setGLType(GL_FLOAT); break;
            case 3: uniform.setSize(1); break;
            }
            const auto queries = mock.contextQueryCount();
            EXPECT_THROW(access.set(.5f), Error);
            EXPECT_EQ(mock.contextQueryCount(), queries); // stale before any GL/context use
            EXPECT_THROW(prepare(context, "scalar"), Error);
            EXPECT_EQ(uniform.getValue<GLint>(), 91);
        }
        EXPECT_TRUE(mock.uniformCalls().empty());
    }

    TEST_F(PreparedFloatUniformTest, ValueOnlyChangesRemainCompatibleAndWrongTargetDoesNotCommit)
    {
        reflect("scalar");
        auto access = prepare(context, "scalar");
        auto &uniform = *context.getUniform("scalar");
        uniform.set(.25f);
        mock.configureCurrentProgram(8);
        EXPECT_THROW(access.set(.5f), Error);
        EXPECT_EQ(uniform.get<GLfloat>(), .25f);
        mock.configureCurrentProgram(0);
        EXPECT_THROW(access.set(.5f), Error);
        EXPECT_EQ(uniform.get<GLfloat>(), .25f);
        mock.configureCurrentProgram(7);
        mock.configureCurrentContext(nullptr);
        EXPECT_THROW(access.set(.5f), Error);
        EXPECT_EQ(uniform.get<GLfloat>(), .25f);
        mock.configureCurrentContext(reinterpret_cast<GLFWwindow *>(&context));
        EXPECT_THROW(access.set(.5f), Error);
        EXPECT_EQ(uniform.get<GLfloat>(), .25f);
        EXPECT_EQ(mock.uniformCalls().size(), 1u);
        mock.configureCurrentContext(reinterpret_cast<GLFWwindow *>(this));
        access.set(.75f);
        EXPECT_EQ(uniform.get<GLfloat>(), .75f);
        EXPECT_EQ(mock.uniformCalls().size(), 2u);
    }

    TEST_F(PreparedFloatUniformTest, MapRejectionsPreserveButSuccessfulPublicationsRevoke)
    {
        reflect("scalar");
        auto access = prepare(context, "scalar");
        EXPECT_THROW(context.addUniform("scalar", nullptr), std::invalid_argument);
        Context::UniformMap invalid;
        invalid.emplace("bad", nullptr);
        EXPECT_THROW(context.replaceUniforms(std::move(invalid)), std::invalid_argument);
        access.set(0);
        context.addUniform("other", std::make_unique<artist::graphic::pipeline::Uniform<API>>());
        EXPECT_THROW(access.set(.5f), Error);
        reflect("scalar");
        auto replaced = prepare(context, "scalar");
        reflect("scalar");
        EXPECT_THROW(replaced.set(.5f), Error);
        auto emptied = prepare(context, "scalar");
        context.replaceUniforms({});
        EXPECT_THROW(emptied.set(.5f), Error);
        EXPECT_EQ(mock.uniformCalls().size(), 1u);
    }

    TEST_F(PreparedFloatUniformTest, ReflectionFailureOfSameProgramPreservesAccess)
    {
        reflect("scalar");
        auto access = prepare(context, "scalar");
        EXPECT_CALL(mock, glGetProgramiv_mock(7, GL_ACTIVE_UNIFORMS, ::testing::_))
            .WillOnce(::testing::SetArgPointee<2>(-1));
        EXPECT_THROW(Reader::on(context), Error);
        access.set(.5f);
        EXPECT_EQ(context.getUniform("scalar")->get<GLfloat>(), .5f);
    }

    TEST_F(PreparedFloatUniformTest, RetirementCannotResurrectWithReusedNumericProgramId)
    {
        reflect("scalar");
        auto old = prepare(context, "scalar");
        EXPECT_CALL(mock, glDeleteProgram_mock(7)).Times(1);
        Context::Freer<artist::graphic::opengl::profile::Pass::Classic>::on(context);
        EXPECT_THROW(old.set(.5f), Error);
        context.setPassID(7);
        EXPECT_THROW(prepare(context, "scalar"), Error);
        EXPECT_CALL(mock, glGetProgramiv_mock(7, GL_ACTIVE_UNIFORMS, ::testing::_))
            .WillOnce(::testing::SetArgPointee<2>(-1));
        EXPECT_THROW(Reader::on(context), Error);
        EXPECT_THROW(old.set(.5f), Error);
        reflect("scalar");
        EXPECT_THROW(old.set(.5f), Error);
        prepare(context, "scalar").set(.25f);
        EXPECT_EQ(mock.uniformCalls().size(), 1u);
    }

    TEST_F(PreparedFloatUniformTest, OwnerDestructionAndUniformMoveAssignmentRejectBeforePointee)
    {
        reflect("scalar");
        auto old = prepare(context, "scalar");
        auto &wrapper = *context.getUniform("scalar");
        auto transferred = std::move(wrapper);
        old.set(.25f); // the unique context, not the wrapper address, is borrowed
        transferred = artist::graphic::pipeline::Uniform<API>();
        const auto queries = mock.contextQueryCount();
        EXPECT_THROW(old.set(.5f), Error);
        EXPECT_EQ(mock.contextQueryCount(), queries);

        reflect("scalar");
        auto deadPass = prepare(context, "scalar");
        context.replaceUniforms({});
        EXPECT_THROW(deadPass.set(.5f), Error);
    }

    // Pass revocation must not rely on uniform or witness destruction.
    TEST_F(PreparedFloatUniformTest, PassDestructionRevokesWithLiveUniformAndWitness)
    {
        auto owner = std::make_unique<Context>();
        owner->setPassID(7);
        reflect("scalar", GL_FLOAT, 1, owner.get());
        auto access = prepare(*owner, "scalar");
        const auto witness = owner->preparedUniformState().lock();
        ASSERT_NE(witness, nullptr);
        const auto generation = witness->generation();
        auto retainedUniform = std::move(*owner->getUniform("scalar"));
        access.set(.25f);
        ASSERT_EQ(mock.uniformCalls().size(), 1u);

        owner.reset();
        EXPECT_FALSE(witness->accepts(generation));
        const auto queries = mock.contextQueryCount();
        for (int attempt = 0; attempt != 2; ++attempt)
        {
            SCOPED_TRACE(attempt);
            try
            {
                access.set(.5f);
                FAIL() << "Expected pass-destruction revocation";
            }
            catch (const Error &error)
            {
                EXPECT_STREQ(error.what(),
                    "ERROR::UNIFORM::SET::STALE_PREPARED_ACCESS");
            }
            EXPECT_EQ(mock.contextQueryCount(), queries);
            EXPECT_EQ(mock.uniformCalls().size(), 1u);
            EXPECT_EQ(retainedUniform.get<GLfloat>(), .25f);
        }
    }

    // A rejected candidate prefix must not revoke the old publication.
    TEST_F(PreparedFloatUniformTest, LateReflectionFailurePreservesPreparedPublication)
    {
        reflect("scalar");
        auto access = prepare(context, "scalar");
        access.set(.25f);
        auto *original = context.getUniform("scalar");
        const auto witness = context.preparedUniformState().lock();
        ASSERT_NE(witness, nullptr);
        const auto generation = witness->generation();

        EXPECT_CALL(mock, glGetProgramiv_mock(7, GL_ACTIVE_UNIFORMS, ::testing::_))
            .WillOnce(::testing::SetArgPointee<2>(2));
        EXPECT_CALL(mock, glGetProgramiv_mock(7, GL_ACTIVE_UNIFORM_MAX_LENGTH, ::testing::_))
            .WillOnce(::testing::SetArgPointee<2>(7));
        for (GLuint index = 0; index != 2; ++index)
        {
            EXPECT_CALL(mock, glGetActiveUniform_mock(7, index, 7,
                ::testing::_, ::testing::_, ::testing::_, ::testing::_))
                .WillOnce(::testing::Invoke([](GLuint, GLuint, GLsizei,
                    GLsizei *length, GLint *size, GLenum *type, GLchar *buffer) {
                    std::memcpy(buffer, "scalar", 7);
                    *length = 6;
                    *size = 1;
                    *type = GL_FLOAT;
                }));
        }
        EXPECT_CALL(mock, glGetUniformLocation_mock(7, ::testing::StrEq("scalar")))
            .WillOnce(::testing::Return(4));
        try
        {
            Reader::on(context);
            FAIL() << "Expected rejection of the second candidate entry";
        }
        catch (const Error &error)
        {
            EXPECT_STREQ(error.what(), "ERROR::PASS::UNIFORM::DUPLICATE_NAME");
        }

        EXPECT_EQ(witness->generation(), generation);
        EXPECT_TRUE(witness->accepts(generation));
        ASSERT_EQ(context.getUniforms().size(), 1u);
        ASSERT_EQ(context.getUniform("scalar"), original);
        EXPECT_EQ(original->get<GLfloat>(), .25f);
        ASSERT_EQ(mock.uniformCalls().size(), 1u);
        access.set(.5f);
        prepare(context, "scalar").set(.75f);
        ASSERT_EQ(mock.uniformCalls().size(), 3u);
        EXPECT_EQ(mock.uniformCalls()[1].location, 3);
        EXPECT_EQ(mock.uniformCalls()[2].location, 3);
        EXPECT_EQ(original->get<GLfloat>(), .75f);
    }

    TEST_F(PreparedFloatUniformTest, RepeatedUpdatesObserveNoCpuNativeOrErasedResolution)
    {
        using Observations = artist::graphic::context::detail::PreparedUniformObservations;
        reflect("scalar");
        const auto preparationC = Observations::nameSearches;
        auto access = prepare(context, "scalar");
        RecordProperty("preparation_cpu_name_searches", std::to_string(Observations::nameSearches - preparationC));
        const auto c0 = Observations::nameSearches;
        auto *uniform = context.getUniform("scalar");
        EXPECT_EQ(Observations::nameSearches - c0, 2u);
        uniform->getContext().setValue(GLfloat(1));
        const auto a0 = Observations::extractions;
        EXPECT_EQ(uniform->get<GLfloat>(), 1);
        EXPECT_EQ(Observations::extractions - a0, 1u);
        EXPECT_CALL(mock, glGetUniformLocation_mock(7, ::testing::StrEq("scalar")))
            .WillOnce(::testing::Return(3));
        EXPECT_EQ(glGetUniformLocation(7, "scalar"), 3); // separate native sensitivity
        EXPECT_CALL(mock, glGetUniformLocation_mock(::testing::_, ::testing::_)).Times(0);
        const auto beforeC = Observations::nameSearches;
        const auto beforeA = Observations::extractions;
        const auto uploads = mock.uniformCalls().size();
        access.set(0);
        access.set(.5f);
        access.set(-.25f);
        EXPECT_EQ(Observations::nameSearches, beforeC);
        EXPECT_EQ(Observations::extractions, beforeA);
        EXPECT_EQ(mock.uniformCalls().size() - uploads, 3u);
        RecordProperty("updates_cpu_name_searches", std::to_string(Observations::nameSearches - beforeC));
        RecordProperty("updates_erased_extractions", std::to_string(Observations::extractions - beforeA));
        RecordProperty("updates_native_locations", "0, verified by strict glGetUniformLocation Times(0) expectation");
        RecordProperty("updates_uploads", std::to_string(mock.uniformCalls().size() - uploads));
        const GLfloat expected[] = {0, .5f, -.25f};
        for (std::size_t index = 0; index != 3; ++index)
        {
            GLfloat actual = 0;
            const auto &call = mock.uniformCalls()[uploads + index];
            EXPECT_EQ(call.location, 3);
            ASSERT_EQ(call.payload.size(), sizeof(actual));
            std::memcpy(&actual, call.payload.data(), sizeof(actual));
            EXPECT_EQ(actual, expected[index]);
        }
        EXPECT_EQ(uniform->get<GLfloat>(), -.25f);
    }
}
#endif
