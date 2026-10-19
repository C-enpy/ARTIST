#ifdef __mock_gl__
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <array>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <opengl/glFunctionMock.hpp>
#include <graphic/pipeline/Uniform.hpp>

namespace
{
    using Context = artist::graphic::api::OpenGL::UniformContext;
    using Uniform = artist::graphic::pipeline::Uniform<artist::graphic::api::OpenGL>;
    using Mock = artist::mock::opengl::glFunctionMock;
    using Error = artist::common::exception::TraceableException<std::runtime_error>;

    // Keep RED executable before the metadata API exists. Its absence is tested
    // independently, not mistaken for a behavioral rejection.
    template <typename C>
    void program(C &context, GLuint id)
    {
        if constexpr (requires { context.setProgramID(id); }) context.setProgramID(id);
    }

    template <typename C>
    void expectProgramMetadata(C &context)
    {
        if constexpr (requires { context.getProgramID(); context.setProgramID(7u); })
        {
            EXPECT_EQ(context.getProgramID(), 0u);
            context.setProgramID(7);
            EXPECT_EQ(context.getProgramID(), 7u);
        }
        else ADD_FAILURE() << "Uniform context needs setProgramID/getProgramID";
    }

    template <typename T> struct Case;
#define UNIFORM_CASE(T, Scalar, Enum, Function, Components, BooleanEnum) \
    template <> struct Case<T> { \
        using scalar = Scalar; \
        static constexpr GLenum type = Enum, booleanType = BooleanEnum; \
        static constexpr const char *function = Function; \
        static constexpr std::size_t components = Components; \
        static constexpr bool fp64 = std::is_same_v<Scalar, GLdouble>; \
    }
    UNIFORM_CASE(GLfloat, GLfloat, GL_FLOAT, "glUniform1f", 1, GL_BOOL);
    UNIFORM_CASE(GLint, GLint, GL_INT, "glUniform1i", 1, GL_BOOL);
    UNIFORM_CASE(GLuint, GLuint, GL_UNSIGNED_INT, "glUniform1ui", 1, GL_BOOL);
    UNIFORM_CASE(GLdouble, GLdouble, GL_DOUBLE, "glUniform1d", 1, 0);
    UNIFORM_CASE(glm::vec2, GLfloat, GL_FLOAT_VEC2, "glUniform2fv", 2, GL_BOOL_VEC2);
    UNIFORM_CASE(glm::vec3, GLfloat, GL_FLOAT_VEC3, "glUniform3fv", 3, GL_BOOL_VEC3);
    UNIFORM_CASE(glm::vec4, GLfloat, GL_FLOAT_VEC4, "glUniform4fv", 4, GL_BOOL_VEC4);
    UNIFORM_CASE(glm::dvec2, GLdouble, GL_DOUBLE_VEC2, "glUniform2dv", 2, 0);
    UNIFORM_CASE(glm::dvec3, GLdouble, GL_DOUBLE_VEC3, "glUniform3dv", 3, 0);
    UNIFORM_CASE(glm::dvec4, GLdouble, GL_DOUBLE_VEC4, "glUniform4dv", 4, 0);
    UNIFORM_CASE(glm::ivec2, GLint, GL_INT_VEC2, "glUniform2iv", 2, GL_BOOL_VEC2);
    UNIFORM_CASE(glm::ivec3, GLint, GL_INT_VEC3, "glUniform3iv", 3, GL_BOOL_VEC3);
    UNIFORM_CASE(glm::ivec4, GLint, GL_INT_VEC4, "glUniform4iv", 4, GL_BOOL_VEC4);
    UNIFORM_CASE(glm::uvec2, GLuint, GL_UNSIGNED_INT_VEC2, "glUniform2uiv", 2, GL_BOOL_VEC2);
    UNIFORM_CASE(glm::uvec3, GLuint, GL_UNSIGNED_INT_VEC3, "glUniform3uiv", 3, GL_BOOL_VEC3);
    UNIFORM_CASE(glm::uvec4, GLuint, GL_UNSIGNED_INT_VEC4, "glUniform4uiv", 4, GL_BOOL_VEC4);
    UNIFORM_CASE(glm::mat2, GLfloat, GL_FLOAT_MAT2, "glUniformMatrix2fv", 4, 0);
    UNIFORM_CASE(glm::mat3, GLfloat, GL_FLOAT_MAT3, "glUniformMatrix3fv", 9, 0);
    UNIFORM_CASE(glm::mat4, GLfloat, GL_FLOAT_MAT4, "glUniformMatrix4fv", 16, 0);
    UNIFORM_CASE(glm::mat2x3, GLfloat, GL_FLOAT_MAT2x3, "glUniformMatrix2x3fv", 6, 0);
    UNIFORM_CASE(glm::mat3x2, GLfloat, GL_FLOAT_MAT3x2, "glUniformMatrix3x2fv", 6, 0);
    UNIFORM_CASE(glm::mat2x4, GLfloat, GL_FLOAT_MAT2x4, "glUniformMatrix2x4fv", 8, 0);
    UNIFORM_CASE(glm::mat4x2, GLfloat, GL_FLOAT_MAT4x2, "glUniformMatrix4x2fv", 8, 0);
    UNIFORM_CASE(glm::mat3x4, GLfloat, GL_FLOAT_MAT3x4, "glUniformMatrix3x4fv", 12, 0);
    UNIFORM_CASE(glm::mat4x3, GLfloat, GL_FLOAT_MAT4x3, "glUniformMatrix4x3fv", 12, 0);
    UNIFORM_CASE(glm::dmat2, GLdouble, GL_DOUBLE_MAT2, "glUniformMatrix2dv", 4, 0);
    UNIFORM_CASE(glm::dmat3, GLdouble, GL_DOUBLE_MAT3, "glUniformMatrix3dv", 9, 0);
    UNIFORM_CASE(glm::dmat4, GLdouble, GL_DOUBLE_MAT4, "glUniformMatrix4dv", 16, 0);
    UNIFORM_CASE(glm::dmat2x3, GLdouble, GL_DOUBLE_MAT2x3, "glUniformMatrix2x3dv", 6, 0);
    UNIFORM_CASE(glm::dmat3x2, GLdouble, GL_DOUBLE_MAT3x2, "glUniformMatrix3x2dv", 6, 0);
    UNIFORM_CASE(glm::dmat2x4, GLdouble, GL_DOUBLE_MAT2x4, "glUniformMatrix2x4dv", 8, 0);
    UNIFORM_CASE(glm::dmat4x2, GLdouble, GL_DOUBLE_MAT4x2, "glUniformMatrix4x2dv", 8, 0);
    UNIFORM_CASE(glm::dmat3x4, GLdouble, GL_DOUBLE_MAT3x4, "glUniformMatrix3x4dv", 12, 0);
    UNIFORM_CASE(glm::dmat4x3, GLdouble, GL_DOUBLE_MAT4x3, "glUniformMatrix4x3dv", 12, 0);
#undef UNIFORM_CASE

    using AllTypes = ::testing::Types<GLfloat, GLint, GLuint, GLdouble,
        glm::vec2, glm::vec3, glm::vec4, glm::dvec2, glm::dvec3, glm::dvec4,
        glm::ivec2, glm::ivec3, glm::ivec4, glm::uvec2, glm::uvec3, glm::uvec4,
        glm::mat2, glm::mat3, glm::mat4, glm::mat2x3, glm::mat3x2, glm::mat2x4,
        glm::mat4x2, glm::mat3x4, glm::mat4x3,
        glm::dmat2, glm::dmat3, glm::dmat4, glm::dmat2x3, glm::dmat3x2,
        glm::dmat2x4, glm::dmat4x2, glm::dmat3x4, glm::dmat4x3>;

    template <typename T> struct Fp64Case;
#define FP64_CASE(T, Entry) \
    template <> struct Fp64Case<T> { static constexpr auto entry = Mock::Fp64EntryPoint::Entry; }
    FP64_CASE(GLdouble, Uniform1d);
    FP64_CASE(glm::dvec2, Uniform2dv);
    FP64_CASE(glm::dvec3, Uniform3dv);
    FP64_CASE(glm::dvec4, Uniform4dv);
    FP64_CASE(glm::dmat2, Matrix2dv);
    FP64_CASE(glm::dmat3, Matrix3dv);
    FP64_CASE(glm::dmat4, Matrix4dv);
    FP64_CASE(glm::dmat2x3, Matrix2x3dv);
    FP64_CASE(glm::dmat3x2, Matrix3x2dv);
    FP64_CASE(glm::dmat2x4, Matrix2x4dv);
    FP64_CASE(glm::dmat4x2, Matrix4x2dv);
    FP64_CASE(glm::dmat3x4, Matrix3x4dv);
    FP64_CASE(glm::dmat4x3, Matrix4x3dv);
#undef FP64_CASE
    using DoubleTypes = ::testing::Types<GLdouble, glm::dvec2, glm::dvec3, glm::dvec4,
        glm::dmat2, glm::dmat3, glm::dmat4, glm::dmat2x3, glm::dmat3x2,
        glm::dmat2x4, glm::dmat4x2, glm::dmat3x4, glm::dmat4x3>;

    class UniformRejectionTest : public ::testing::Test
    {
    protected:
        Mock &mock = *Mock::instance();
        void SetUp() override
        {
            mock.resetContractState();
            mock.configureCurrentContext(reinterpret_cast<GLFWwindow *>(this)); // Opaque, never dereferenced.
            mock.configureCurrentProgram(7);
            mock.configureFp64(true, false, true);
        }
        void TearDown() override
        {
            Mock::reset();
            mock.resetContractState();
        }
        Uniform make(GLenum type, GLuint size = 1)
        {
            auto context = std::make_unique<Context>();
            context->setUniformID(42);
            context->setGLType(type);
            context->setSize(size);
            program(*context, 7);
            return Uniform(std::move(context));
        }
        template <typename T>
        void rejectAndKeep(Uniform &uniform, const T &value, const char *reason)
        {
            uniform.getContext().setValue(GLint(101));
            const auto uploads = mock.uniformCalls().size();
            try { uniform.set(value); FAIL() << "expected rejection: " << reason; }
            catch (const Error &error) { EXPECT_THAT(error.what(), ::testing::HasSubstr(reason)); }
            EXPECT_EQ(mock.uniformCalls().size(), uploads);
            ASSERT_NO_THROW((void)uniform.get<GLint>());
            EXPECT_EQ(uniform.get<GLint>(), 101);
        }
    };

    template <typename T> class UniformFamiliesTest : public UniformRejectionTest {};
    TYPED_TEST_SUITE(UniformFamiliesTest, AllTypes);

    TYPED_TEST(UniformFamiliesTest, MatchingTypeUploadsOneOwnedSnapshot)
    {
        using C = Case<TypeParam>;
        auto uniform = this->make(C::type);
        const TypeParam value(3);
        ASSERT_NO_THROW(uniform.set(value));
        ASSERT_EQ(this->mock.uniformCalls().size(), 1u);
        const auto &call = this->mock.uniformCalls().front();
        EXPECT_EQ(call.function, C::function);
        EXPECT_EQ(call.location, 42);
        EXPECT_EQ(call.count, 1);
        EXPECT_EQ(call.transpose, GL_FALSE);
        EXPECT_EQ(call.components, C::components);
        EXPECT_EQ(call.scalarBytes, sizeof(typename C::scalar));
        std::array<typename C::scalar, C::components> actual{};
        ASSERT_EQ(call.payload.size(), sizeof(actual));
        std::memcpy(actual.data(), call.payload.data(), sizeof(actual));
        const auto *expected = [&] {
            if constexpr (std::is_arithmetic_v<TypeParam>) return &value;
            else return glm::value_ptr(value);
        }();
        for (std::size_t i = 0; i < actual.size(); ++i) EXPECT_EQ(actual[i], expected[i]);
        EXPECT_EQ(uniform.template get<TypeParam>(), value);
    }

    TYPED_TEST(UniformFamiliesTest, InvalidSizePreservesThePreviousTypeAndValue)
    {
        for (GLuint size : {0u, 2u})
        {
            auto uniform = this->make(Case<TypeParam>::type, size);
            this->rejectAndKeep(uniform, TypeParam(3), "ARRAY_NOT_SUPPORTED");
        }
    }

    TYPED_TEST(UniformFamiliesTest, IncompatibleTypePreservesThePreviousTypeAndValue)
    {
        // All 34 CPU families reject a texture/image object type.
        auto uniform = this->make(GL_IMAGE_2D);
        this->rejectAndKeep(uniform, TypeParam(3), "TYPE_MISMATCH");
    }

    TYPED_TEST(UniformFamiliesTest, BooleanCompatibilityIsNotNaiveEnumEquality)
    {
        if constexpr (Case<TypeParam>::booleanType != 0)
        {
            auto uniform = this->make(Case<TypeParam>::booleanType);
            const TypeParam value(3); // CPU value must not be normalized to one.
            ASSERT_NO_THROW(uniform.set(value));
            EXPECT_EQ(uniform.template get<TypeParam>(), value);
            ASSERT_EQ(this->mock.uniformCalls().size(), 1u);
            EXPECT_EQ(this->mock.uniformCalls().front().function, Case<TypeParam>::function);
        }
        else
        {
            auto uniform = this->make(GL_BOOL);
            this->rejectAndKeep(uniform, TypeParam(3), "TYPE_MISMATCH");
        }
    }

    template <typename T> class Fp64UniformTest : public UniformRejectionTest {};
    TYPED_TEST_SUITE(Fp64UniformTest, DoubleTypes);

    TYPED_TEST(Fp64UniformTest, RequiresCapabilityAndTheSelectedEntryPoint)
    {
        auto uniform = this->make(Case<TypeParam>::type);
        this->mock.configureFp64(false, false, true);
        this->rejectAndKeep(uniform, TypeParam(3), "FP64");
        this->mock.configureFp64(true, false, true);
        this->mock.configureFp64EntryPoint(Fp64Case<TypeParam>::entry, false);
        this->rejectAndKeep(uniform, TypeParam(3), "FP64");
        this->mock.configureFp64(false, true, true);
        ASSERT_NO_THROW(uniform.set(TypeParam(3)));
        this->mock.configureFp64(true, false, true);
        ASSERT_NO_THROW(uniform.set(TypeParam(5)));
        EXPECT_EQ(this->mock.uniformCalls().size(), 2u);
    }

    TEST_F(UniformRejectionTest, RejectionKeepsAnUninitializedValueUninitialized)
    {
        auto uniform = make(GL_INT);
        EXPECT_THROW(uniform.set(2.5f), Error);
        EXPECT_TRUE(mock.uniformCalls().empty());
        try { (void)uniform.get<GLfloat>(); FAIL() << "expected uninitialized"; }
        catch (const Error &error) { EXPECT_THAT(error.what(), ::testing::HasSubstr("UNINITIALIZED")); }
    }

    TEST_F(UniformRejectionTest, NoCurrentContextRejectsBeforeAnyGLQuery)
    {
        auto uniform = make(GL_FLOAT);
        mock.configureCurrentContext(nullptr);
        rejectAndKeep(uniform, 2.5f, "CONTEXT");
        EXPECT_TRUE(mock.glCalls().empty());
    }

    TEST_F(UniformRejectionTest, NoCurrentProgramAndDifferentProgramRejectBeforeMutation)
    {
        auto uniform = make(GL_FLOAT);
        for (GLint current : {0, 8})
        {
            mock.configureCurrentProgram(current);
            rejectAndKeep(uniform, 2.5f, "PROGRAM");
        }
    }

    TEST_F(UniformRejectionTest, SameLocationDoesNotAuthorizeAnotherProgram)
    {
        auto target = make(GL_FLOAT);
        auto decoy = make(GL_FLOAT);
        program(decoy.getContext(), 8);
        mock.configureCurrentProgram(8);
        rejectAndKeep(target, 2.5f, "PROGRAM");
        ASSERT_NO_THROW(decoy.set(8.5f));
        ASSERT_EQ(mock.uniformCalls().size(), 1u);
        EXPECT_EQ(decoy.get<GLfloat>(), 8.5f);
    }

    TEST_F(UniformRejectionTest, ManualTypeAndProgramMetadataAndOneElementRemainUsable)
    {
        Context defaults;
        EXPECT_EQ(defaults.getUniformID(), 0u);
        EXPECT_EQ(defaults.getGLType(), 0u);
        EXPECT_EQ(defaults.getGLSize(), 1u);
        expectProgramMetadata(defaults);
        Uniform uniform;
        ASSERT_NO_THROW(uniform.set(glm::vec3(3)));
        ASSERT_EQ(mock.uniformCalls().size(), 1u);
        EXPECT_EQ(mock.uniformCalls().front().count, 1);
    }

    TEST_F(UniformRejectionTest, ScalarIntegerAcceptsSamplerFamiliesButUnsignedAndVectorsDoNot)
    {
        const GLenum samplers[] = {
            GL_SAMPLER_1D, GL_SAMPLER_2D, GL_SAMPLER_3D, GL_SAMPLER_CUBE,
            GL_SAMPLER_1D_SHADOW, GL_SAMPLER_2D_SHADOW, GL_SAMPLER_CUBE_SHADOW,
            GL_SAMPLER_1D_ARRAY, GL_SAMPLER_2D_ARRAY, GL_SAMPLER_1D_ARRAY_SHADOW,
            GL_SAMPLER_2D_ARRAY_SHADOW, GL_SAMPLER_2D_MULTISAMPLE,
            GL_SAMPLER_2D_MULTISAMPLE_ARRAY, GL_SAMPLER_BUFFER, GL_SAMPLER_2D_RECT,
            GL_SAMPLER_2D_RECT_SHADOW, GL_INT_SAMPLER_1D, GL_INT_SAMPLER_2D,
            GL_INT_SAMPLER_3D, GL_INT_SAMPLER_CUBE, GL_INT_SAMPLER_1D_ARRAY,
            GL_INT_SAMPLER_2D_ARRAY, GL_INT_SAMPLER_2D_MULTISAMPLE,
            GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, GL_INT_SAMPLER_BUFFER, GL_INT_SAMPLER_2D_RECT,
            GL_UNSIGNED_INT_SAMPLER_1D, GL_UNSIGNED_INT_SAMPLER_2D, GL_UNSIGNED_INT_SAMPLER_3D,
            GL_UNSIGNED_INT_SAMPLER_CUBE, GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,
            GL_UNSIGNED_INT_SAMPLER_2D_ARRAY, GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE,
            GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, GL_UNSIGNED_INT_SAMPLER_BUFFER,
            GL_UNSIGNED_INT_SAMPLER_2D_RECT, GL_SAMPLER_CUBE_MAP_ARRAY,
            GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW, GL_INT_SAMPLER_CUBE_MAP_ARRAY,
            GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY};
        for (GLenum sampler : samplers)
        {
            SCOPED_TRACE(sampler);
            auto uniform = make(sampler);
            ASSERT_NO_THROW(uniform.set(GLint(3)));
            EXPECT_EQ(mock.uniformCalls().back().function, "glUniform1i");
            rejectAndKeep(uniform, GLuint(3), "TYPE_MISMATCH");
            rejectAndKeep(uniform, glm::ivec2(3), "TYPE_MISMATCH");
        }
    }

    TEST_F(UniformRejectionTest, NoImplicitNumericConversionOrMatrixTranspose)
    {
        auto integer = make(GL_INT);
        rejectAndKeep(integer, GLfloat(3), "TYPE_MISMATCH");
        rejectAndKeep(integer, GLuint(3), "TYPE_MISMATCH");
        auto unsignedInteger = make(GL_UNSIGNED_INT);
        rejectAndKeep(unsignedInteger, GLint(3), "TYPE_MISMATCH");
        auto floating = make(GL_FLOAT);
        rejectAndKeep(floating, GLint(3), "TYPE_MISMATCH");
        rejectAndKeep(floating, GLdouble(3), "TYPE_MISMATCH");
        auto doubleValue = make(GL_DOUBLE);
        rejectAndKeep(doubleValue, GLfloat(3), "TYPE_MISMATCH");
        auto matrix = make(GL_FLOAT_MAT3x2);
        rejectAndKeep(matrix, glm::mat2x3(3), "TYPE_MISMATCH");
        rejectAndKeep(matrix, glm::dmat3x2(3), "TYPE_MISMATCH");
        auto vector = make(GL_FLOAT_VEC3);
        rejectAndKeep(vector, glm::vec2(3), "TYPE_MISMATCH");
    }

    TEST_F(UniformRejectionTest, Fp64NeedsCoreOrExtensionAndTheSelectedEntryPoint)
    {
        auto uniform = make(GL_DOUBLE);
        mock.configureFp64(false, false, true);
        rejectAndKeep(uniform, GLdouble(3), "FP64");
        mock.configureFp64(true, false, false);
        rejectAndKeep(uniform, GLdouble(3), "FP64");
        mock.configureFp64(false, true, true);
        ASSERT_NO_THROW(uniform.set(GLdouble(3)));
        mock.configureFp64(true, false, true);
        ASSERT_NO_THROW(uniform.set(GLdouble(5)));
        EXPECT_EQ(mock.uniformCalls().size(), 2u);

        auto matrix = make(GL_DOUBLE_MAT2x3);
        mock.configureFp64EntryPoint(Mock::Fp64EntryPoint::Matrix2x3dv, false);
        rejectAndKeep(matrix, glm::dmat2x3(3), "FP64");
        // A missing unrelated function must not reject a supported scalar.
        ASSERT_NO_THROW(uniform.set(GLdouble(7)));
    }

    TEST_F(UniformRejectionTest, MissingOnIsAnExplicitUnsupportedTypeNotAnIllFormedSet)
    {
        auto uniform = make(GL_BOOL);
        rejectAndKeep(uniform, true, "UNSUPPORTED_TYPE");
        rejectAndKeep(uniform, glm::bvec3(true), "UNSUPPORTED_TYPE");
        rejectAndKeep(uniform, std::string("unsupported"), "UNSUPPORTED_TYPE");
        EXPECT_TRUE(mock.glCalls().empty());
    }

    TEST_F(UniformRejectionTest, MovedFromUnsupportedTypeRejectsBeforeDispatchAndPreservesDestination)
    {
        auto source = make(GL_INT);
        source.getContext().setValue<GLint>(101);
        const auto *context = &source.getContext();
        const auto *value = &source.get<GLint>();
        Uniform destination(std::move(source));

        try { source.set<bool>(true); FAIL() << "expected moved-from rejection"; }
        catch (const Error &error)
        {
            EXPECT_THAT(error.what(), ::testing::HasSubstr("ERROR::UNIFORM::FREED_CONTEXT"));
            EXPECT_THAT(error.what(), ::testing::Not(::testing::HasSubstr("UNSUPPORTED_TYPE")));
        }

        EXPECT_TRUE(mock.glCalls().empty());
        EXPECT_TRUE(mock.uniformCalls().empty());
        EXPECT_EQ(&destination.getContext(), context);
        ASSERT_NO_THROW((void)destination.get<GLint>());
        EXPECT_EQ(&destination.get<GLint>(), value);
        EXPECT_EQ(destination.get<GLint>(), 101);
    }

    TEST_F(UniformRejectionTest, DirectSetterStillChecksMetadataAndNativePreconditions)
    {
        using Setter = artist::graphic::opengl::pipeline::component::uniform::OpenGLUniformSetter<GLfloat>;
        auto uniform = make(GL_INT);
        uniform.getContext().setValue(2.5f);
        EXPECT_THROW(Setter::on(uniform.getContext()), Error);
        uniform.getContext().setGLType(GL_FLOAT);
        mock.configureCurrentProgram(8);
        EXPECT_THROW(Setter::on(uniform.getContext()), Error);
        EXPECT_TRUE(mock.uniformCalls().empty());
        EXPECT_EQ(uniform.get<GLfloat>(), 2.5f);
    }
}
#endif
