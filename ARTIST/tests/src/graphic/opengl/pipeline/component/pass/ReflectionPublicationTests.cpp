#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <memory>
#include <string>
#include <vector>
#include <opengl/glFunctionMock.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/opengl/context/PassContext.hpp>
#include <graphic/opengl/pipeline/component/attribute/Binder.hpp>
#include <graphic/opengl/pipeline/component/attribute/Setter.hpp>
#include <graphic/opengl/pipeline/component/attribute/Unbinder.hpp>
#include <graphic/opengl/pipeline/component/pass/Loader.hpp>
#include <graphic/opengl/pipeline/component/pass/Freer.hpp>
#include <graphic/opengl/pipeline/component/pass/ShaderAttacher.hpp>
#include <graphic/opengl/pipeline/component/pass/UniformReader.hpp>
#include <graphic/opengl/pipeline/component/pass/AttributeReader.hpp>
#include <graphic/opengl/pipeline/component/pass/User.hpp>
#include <graphic/pipeline/Pass.hpp>
#include <TestUtils.hpp>

namespace
{
    using GL = artist::graphic::api::OpenGL;
    using Mock = artist::mock::opengl::glFunctionMock;
    using Attribute = artist::graphic::pipeline::Attribute<GL, artist::graphic::opengl::profile::Attribute::Classic>;
    using Uniform = artist::graphic::pipeline::Uniform<GL>;
    using Pass = artist::graphic::pipeline::Pass<GL, artist::graphic::opengl::profile::Pass::Classic>;
    using AttributeReader = GL::PassContext::AttributeReader<artist::graphic::opengl::profile::Pass::Classic>;
    using UniformReader = GL::PassContext::UniformReader<artist::graphic::opengl::profile::Pass::Classic>;
    using artist::test::utils::expectSpecificError;
    using Error = artist::common::exception::TraceableException<std::runtime_error>;

    // Opaque context identities; the mock never dereferences a GLFW window.
    GLFWwindow *contextA() { static int identity; return reinterpret_cast<GLFWwindow *>(&identity); }
    GLFWwindow *contextB() { static int identity; return reinterpret_cast<GLFWwindow *>(&identity); }

    struct RestoreCurrentContext
    {
        ~RestoreCurrentContext() { Mock::instance()->configureCurrentContext(contextA()); }
    };

    void writeName(const std::string &name, GLsizei capacity, GLsizei *length, GLchar *buffer)
    {
        ASSERT_GT(capacity, static_cast<GLsizei>(name.size()));
        std::memcpy(buffer, name.data(), name.size());
        buffer[name.size()] = '\0';
        *length = static_cast<GLsizei>(name.size());
    }

    void reflectAttributes(const std::vector<std::string> &names, bool lateInvalidLocation = false)
    {
        auto &gl = *Mock::instance();
        EXPECT_CALL(gl, glGetProgramiv_mock(1, GL_ACTIVE_ATTRIBUTES, ::testing::_))
            .WillOnce(::testing::SetArgPointee<2>(static_cast<GLint>(names.size())));
        EXPECT_CALL(gl, glGetProgramiv_mock(1, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, ::testing::_))
            .Times(names.empty() ? 0 : 1).WillRepeatedly(::testing::SetArgPointee<2>(64));
        for (GLuint i = 0; i < names.size(); ++i)
        {
            EXPECT_CALL(gl, glGetActiveAttrib_mock(1, i, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
                .WillOnce([name = names[i]](GLuint, GLuint, GLsizei capacity, GLsizei *length, GLint *size, GLenum *type, GLchar *buffer) {
                    writeName(name, capacity, length, buffer);
                    *size = 1;
                    *type = GL_FLOAT_VEC3;
                });
        }
        EXPECT_CALL(gl, glGetAttribLocation_mock(1, ::testing::_)).Times(names.size())
            .WillRepeatedly([names, lateInvalidLocation](GLuint, const GLchar *name) -> GLint {
                const auto index = std::find(names.begin(), names.end(), name) - names.begin();
                return lateInvalidLocation && index == 1 ? -1 : static_cast<GLint>(index);
            });
    }

    class ReflectionPublicationTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            Mock::instance()->resetContractState();
            Mock::instance()->configureCurrentContext(contextA());
            EXPECT_CALL(*Mock::instance(), glDeleteBuffers_mock(::testing::_, ::testing::_)).Times(::testing::AnyNumber());
        }
        void TearDown() override
        {
            Mock::reset();
            Mock::instance()->resetContractState();
        }
        std::unique_ptr<Attribute> ownedAttribute(GLuint buffer)
        {
            auto attribute = std::make_unique<Attribute>();
            attribute->getContext().setBufferID(buffer);
            return attribute;
        }
        std::vector<GLuint> deletedNames() const
        {
            std::vector<GLuint> result;
            for (const auto &call : Mock::instance()->deleteBufferCalls())
            {
                EXPECT_EQ(call.count, 1);
                result.insert(result.end(), call.names.begin(), call.names.end());
            }
            std::sort(result.begin(), result.end());
            return result;
        }
    };

    TEST_F(ReflectionPublicationTests, SuccessfulReaderCommitRetiresOldVboAndMissingNamesBeforeReturn)
    {
        GL::PassContext context;
        context.setPassID(1);
        context.addAttribute("obsolete", ownedAttribute(71));
        context.addAttribute("reloaded", ownedAttribute(72));
        auto *retainedBorrow = context.getAttribute("reloaded");
        reflectAttributes({"reloaded"});
        ASSERT_NO_THROW(AttributeReader::on(context));
        EXPECT_EQ(deletedNames(), (std::vector<GLuint>{71, 72}));
        EXPECT_EQ(context.getAttribute("obsolete"), nullptr);
        ASSERT_EQ(context.getAttributes().size(), 1u);
        EXPECT_EQ(context.getAttribute("reloaded")->getContext().getBufferID(), 0u);
        // K2: commit réussi invalide les borrows anciens, nom retenu inclus.
        EXPECT_NE(context.getAttribute("reloaded"), retainedBorrow);
    }

    TEST_F(ReflectionPublicationTests, EmptyReaderCommitRetiresAllOldVbosBeforeReturn)
    {
        GL::PassContext context;
        context.setPassID(1);
        context.addAttribute("first", ownedAttribute(73));
        context.addAttribute("second", ownedAttribute(74));
        reflectAttributes({});
        ASSERT_NO_THROW(AttributeReader::on(context));
        EXPECT_TRUE(context.getAttributes().empty());
        EXPECT_EQ(deletedNames(), (std::vector<GLuint>{73, 74}));
    }

    TEST_F(ReflectionPublicationTests, LateReaderFailureKeepsOldVboAndPublishesNoPrefix)
    {
        GL::PassContext context;
        context.setPassID(1);
        context.addAttribute("old", ownedAttribute(75));
        auto *old = context.getAttribute("old");
        reflectAttributes({"firstNew", "badNew"}, true);
        expectSpecificError([&] { AttributeReader::on(context); }, Error("ERROR::PASS::ATTRIBUTE::INVALID_LOCATION"));
        ASSERT_EQ(context.getAttributes().size(), 1u);
        ASSERT_EQ(context.getAttribute("old"), old);
        EXPECT_EQ(old->getContext().getBufferID(), 75u);
        EXPECT_EQ(context.getAttribute("firstNew"), nullptr);
        EXPECT_TRUE(deletedNames().empty());
    }

    TEST_F(ReflectionPublicationTests, EmptyCommitValidatesEveryOldOwnerBeforeAnyRetirement)
    {
        GL::PassContext context;
        context.setPassID(1);
        context.addAttribute("one", std::make_unique<Attribute>());
        context.addAttribute("two", std::make_unique<Attribute>());
        // Pick map traversal order: a release-as-you-validate implementation
        // would retire the valid first entry before reaching the invalid one.
        auto entry = context.getAttributes().begin();
        const auto firstName = entry->first;
        auto *first = entry->second.get();
        auto *second = (++entry)->second.get();
        const auto secondName = entry->first;
        first->getContext().setBufferID(76);
        Mock::instance()->configureCurrentContext(contextB());
        second->getContext().setBufferID(77);
        Mock::instance()->configureCurrentContext(contextA());
        reflectAttributes({});
        expectSpecificError([&] { AttributeReader::on(context); }, Error("ERROR::ATTRIBUTE::CONTEXT_MISMATCH"));
        EXPECT_EQ(context.getAttributes().size(), 2u);
        EXPECT_TRUE(deletedNames().empty());
        // Retire each identity explicitly under its owner context, even if an
        // assertion failed; the map's destruction must not hide the verdict.
        if (auto *remaining = context.getAttribute(firstName)) remaining->free();
        Mock::instance()->configureCurrentContext(contextB());
        RestoreCurrentContext restore;
        if (auto *remaining = context.getAttribute(secondName)) remaining->free();
    }

    TEST_F(ReflectionPublicationTests, NonemptyCommitRejectsWrongOrMissingContextWithoutPublishingCandidate)
    {
        for (auto *current : {contextB(), static_cast<GLFWwindow *>(nullptr)})
        {
            SCOPED_TRACE(current ? "wrong context" : "no context");
            GL::PassContext context;
            context.setPassID(1);
            context.addAttribute("old", ownedAttribute(80));
            auto *old = context.getAttribute("old");
            reflectAttributes({"new"});
            Mock::instance()->configureCurrentContext(current);
            {
                RestoreCurrentContext restore;
                expectSpecificError([&] { AttributeReader::on(context); }, Error(current
                    ? "ERROR::ATTRIBUTE::CONTEXT_MISMATCH" : "ERROR::ATTRIBUTE::NO_CURRENT_CONTEXT"));
            }
            EXPECT_EQ(context.getAttributes().size(), 1u);
            EXPECT_EQ(context.getAttribute("old"), old);
            EXPECT_EQ(context.getAttribute("new"), nullptr);
            EXPECT_TRUE(deletedNames().empty());
            old->free();
            // Keep the strict journal relative to this attempt, not the cleanup.
            ASSERT_EQ(deletedNames(), (std::vector<GLuint>{80}));
            Mock::instance()->resetContractState();
            Mock::instance()->configureCurrentContext(contextA());
        }
    }

    // The old assignment terminates in the destructor. Isolate it and require
    // recovery, not death; a controlled exit avoids a Windows CRT dialog.
    TEST_F(ReflectionPublicationTests, PointReplacementRejectsWrongContextBeforeDestroyingOldOwner)
    {
#if GTEST_HAS_DEATH_TEST
        EXPECT_EXIT({
            std::set_terminate([] {
                std::fputs("POINT_REPLACE_TERMINATED", stderr);
                std::fflush(stderr);
                std::_Exit(86);
            });
            bool recovered = false;
            bool unchanged = false;
            {
                GL::PassContext context;
                context.addAttribute("old", ownedAttribute(78));
                auto *old = context.getAttribute("old");
                auto replacement = std::make_unique<Attribute>();
                Mock::instance()->configureCurrentContext(contextB());
                try { context.addAttribute("old", std::move(replacement)); }
                catch (const std::runtime_error &error)
                {
                    recovered = std::string(error.what()).find("ERROR::ATTRIBUTE::CONTEXT_MISMATCH") != std::string::npos;
                }
                unchanged = context.getAttribute("old") == old
                    && old->getContext().getBufferID() == 78u
                    && Mock::instance()->glCalls().empty();
                Mock::instance()->configureCurrentContext(contextA());
            }
            const auto &deletions = Mock::instance()->deleteBufferCalls();
            const bool retiredOnce = deletions.size() == 1 && deletions[0].count == 1
                && deletions[0].names.size() == 1 && deletions[0].names[0] == 78u;
            const bool success = recovered && unchanged && retiredOnce;
            std::fputs(success ? "POINT_REPLACE_RECOVERED" : "POINT_REPLACE_INVALID_STATE", stderr);
            std::fflush(stderr);
            std::_Exit(success ? 0 : 87);
        }, ::testing::ExitedWithCode(0), "POINT_REPLACE_RECOVERED");
#else
        GTEST_SKIP() << "Point replacement recovery requires process isolation on this toolchain";
#endif
    }

    TEST_F(ReflectionPublicationTests, PassLoadCommitsUniformsEvenWhenAttributeReaderLaterFails)
    {
        Pass pass(std::make_unique<GL::PassContext>());
        auto &context = pass.getContext();
        context.addUniform("oldUniform", std::make_unique<Uniform>());
        context.addAttribute("oldAttribute", ownedAttribute(79));
        auto *oldAttribute = context.getAttribute("oldAttribute");
        auto &gl = *Mock::instance();
        EXPECT_CALL(gl, glCreateProgram_mock()).WillOnce(::testing::Return(1));
        EXPECT_CALL(gl, glLinkProgram_mock(1)).Times(1);
        EXPECT_CALL(gl, glGetProgramiv_mock(1, GL_LINK_STATUS, ::testing::_)).WillOnce(::testing::SetArgPointee<2>(GL_TRUE));
        EXPECT_CALL(gl, glDeleteProgram_mock(1)).Times(1);
        EXPECT_CALL(gl, glGetProgramiv_mock(1, GL_ACTIVE_UNIFORMS, ::testing::_)).WillOnce(::testing::SetArgPointee<2>(1));
        EXPECT_CALL(gl, glGetProgramiv_mock(1, GL_ACTIVE_UNIFORM_MAX_LENGTH, ::testing::_)).WillOnce(::testing::SetArgPointee<2>(64));
        EXPECT_CALL(gl, glGetActiveUniform_mock(1, 0, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
            .WillOnce([](GLuint, GLuint, GLsizei capacity, GLsizei *length, GLint *size, GLenum *type, GLchar *buffer) {
                writeName("newUniform", capacity, length, buffer);
                *size = 1;
                *type = GL_INT;
            });
        EXPECT_CALL(gl, glGetUniformLocation_mock(1, ::testing::_)).WillOnce(::testing::Return(3));
        reflectAttributes({"firstNew", "badNew"}, true);

        expectSpecificError([&] { pass.load(); }, Error("ERROR::PASS::ATTRIBUTE::INVALID_LOCATION"));
        ASSERT_EQ(context.getUniforms().size(), 1u);
        EXPECT_EQ(context.getUniform("oldUniform"), nullptr);
        ASSERT_NE(context.getUniform("newUniform"), nullptr);
        EXPECT_EQ(context.getUniform("newUniform")->getContext().getProgramID(), 1u);
        ASSERT_EQ(context.getAttributes().size(), 1u);
        EXPECT_EQ(context.getAttribute("oldAttribute"), oldAttribute);
        EXPECT_EQ(context.getPassID(), 1u); // No rollback of the linked program.
        EXPECT_TRUE(deletedNames().empty());
    }
}

#endif
