#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <array>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <fstream>
#include <iterator>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <vector>
#include <glm/glm.hpp>
#include <graphic/Api.hpp>
#include <graphic/opengl/context/AttributeContext.hpp>
#include <graphic/opengl/profile/Attribute.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/opengl/pipeline/component/attribute/Binder.hpp>
#include <graphic/opengl/pipeline/component/attribute/Setter.hpp>
#include <graphic/opengl/pipeline/component/attribute/Unbinder.hpp>
#include <graphic/opengl/pipeline/component/pass/AttributeReader.hpp>
#include <graphic/pipeline/Attribute.hpp>
#include <common/exception/TraceableException.hpp>
#include <OpenGLComponentTests.hpp>
#include <TestUtils.hpp>

namespace api = artist::graphic::api;
namespace pipeline = artist::graphic::pipeline;
using artist::common::exception::TraceableException;
using artist::graphic::opengl::profile::Attribute::Classic;
using artist::test::utils::expectSpecificError;

namespace
{
    using Context = api::OpenGL::AttributeContext;
    using Attribute = pipeline::Attribute<api::OpenGL, Classic>;

    /// Every field read by the components is set; buffer 0 lets the binder create the VBO.
    std::unique_ptr<Context> makeContext(GLuint components, GLenum type)
    {
        auto context = std::make_unique<Context>();
        context->setAttributeID(0);
        context->setGLSize(components);
        context->setGLType(type);
        context->setGLUsage(GL_STATIC_DRAW);
        context->setBufferID(0);
        return context;
    }

    GLint boundArrayBufferSize()
    {
        GLint size = -1;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        return size;
    }

    GLint attributeStride(GLuint index)
    {
        GLint stride = -1;
        glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);
        return stride;
    }

    GLint boundArrayBuffer()
    {
        GLint buffer = -1;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &buffer);
        return buffer;
    }

    template <typename Owner>
    void releaseAttribute(Owner &owner)
    {
        if constexpr (requires { owner.free(); }) owner.free();
        else ADD_FAILURE() << "K1 requires public Attribute::free()";
    }
}

class AttributeComponentTest : public OpenGLComponentTest
{
protected:
    GLuint vertexArray = 0;

    void SetUp() override
    {
        OpenGLComponentTest::SetUp();
        // A core context rejects glVertexAttribPointer without a bound vertex array.
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        // GLEW initialization leaves GL_INVALID_ENUM on core profiles.
        for (int i = 0; i < 16 && glGetError() != GL_NO_ERROR; ++i)
        {
        }
    }

    void TearDown() override
    {
        if (vertexArray) glDeleteVertexArrays(1, &vertexArray);
        OpenGLComponentTest::TearDown();
    }
};

TEST_F(AttributeComponentTest, NativeOwnerUploadFreeRecreateAndDestructorRetireNames)
{
    GLuint first = 0, second = 0;
    {
        Attribute owner(makeContext(1, GL_FLOAT));
        auto value = std::make_shared<float>(7.25f);
        owner.bind(); first = owner.getContext().getBufferID();
        ASSERT_NE(first, 0u);
        owner.set(value);
        float actual = 0;
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(actual), &actual);
        ASSERT_EQ(actual, *value);
        ASSERT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
        releaseAttribute(owner); releaseAttribute(owner);
        EXPECT_EQ(owner.getContext().getBufferID(), 0u);
        EXPECT_EQ(owner.getContext().getValue<float>(), value);
        EXPECT_EQ(glIsBuffer(first), GL_FALSE);
        owner.bind(); second = owner.getContext().getBufferID();
        ASSERT_NE(second, 0u);
        EXPECT_EQ(glIsBuffer(second), GL_TRUE);
        owner.set(value);
    }
    EXPECT_EQ(glIsBuffer(second), GL_FALSE);
    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
}

/// A5: bind, set<int> and unbind on a real context upload exactly one int.
TEST_F(AttributeComponentTest, SetIntUploadsOneIntWithoutGLError)
{
    Attribute attribute(makeContext(1, GL_INT));

    ASSERT_NO_THROW(attribute.bind());
    ASSERT_NO_THROW(attribute.set(std::make_shared<int>(7)));

    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
    EXPECT_EQ(boundArrayBufferSize(), static_cast<GLint>(sizeof(int)));
    EXPECT_EQ(attributeStride(0), static_cast<GLint>(sizeof(int)));

    ASSERT_NO_THROW(attribute.unbind());
    EXPECT_EQ(boundArrayBuffer(), 0);
    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
}

/// A5: a three-component element is uploaded with its own size, not a pointer's.
TEST_F(AttributeComponentTest, SetVec3UploadsOneVec3WithoutGLError)
{
    Attribute attribute(makeContext(1, GL_FLOAT_VEC3));

    ASSERT_NO_THROW(attribute.bind());
    ASSERT_NO_THROW(attribute.set(std::make_shared<glm::vec3>(1.0f, 2.0f, 3.0f)));

    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
    EXPECT_EQ(boundArrayBufferSize(), static_cast<GLint>(sizeof(glm::vec3)));
    EXPECT_EQ(attributeStride(0), static_cast<GLint>(sizeof(glm::vec3)));

    ASSERT_NO_THROW(attribute.unbind());
    EXPECT_EQ(boundArrayBuffer(), 0);
    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
}

/// A3: a null value is refused before the driver allocates or describes anything.
TEST_F(AttributeComponentTest, SetNullValueIsRejectedBeforeUpload)
{
    Attribute attribute(makeContext(1, GL_INT));

    ASSERT_NO_THROW(attribute.bind());
    expectSpecificError([&attribute]
                        { attribute.set(std::shared_ptr<int>{}); },
                        TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::SET::NULL_VALUE"));

    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
    EXPECT_EQ(boundArrayBufferSize(), 0);
    EXPECT_EQ(attributeStride(0), 0);

    ASSERT_NO_THROW(attribute.unbind());
    EXPECT_EQ(boundArrayBuffer(), 0);
    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
}

namespace
{
    // Setup proof, not a production-trait test. Diverging aliases need arbitration.
    static_assert(std::is_same_v<float, GLfloat>);
    static_assert(std::is_same_v<std::int32_t, GLint> && std::is_same_v<std::int32_t, int>);
    static_assert(std::is_same_v<std::int32_t, glm::int32>);
    static_assert(std::is_same_v<std::uint32_t, GLuint> && std::is_same_v<std::uint32_t, unsigned>);
    static_assert(std::is_same_v<std::uint32_t, glm::uint32>);
    static_assert(std::is_same_v<glm::vec2, glm::highp_vec2> && std::is_same_v<glm::vec3, glm::highp_vec3> && std::is_same_v<glm::vec4, glm::highp_vec4>);
    static_assert(std::is_same_v<glm::ivec2, glm::highp_ivec2> && std::is_same_v<glm::ivec3, glm::highp_ivec3> && std::is_same_v<glm::ivec4, glm::highp_ivec4>);
    static_assert(std::is_same_v<glm::uvec2, glm::highp_uvec2> && std::is_same_v<glm::uvec3, glm::highp_uvec3> && std::is_same_v<glm::uvec4, glm::highp_uvec4>);
    static_assert(std::is_same_v<glm::ivec2, glm::i32vec2> && std::is_same_v<glm::ivec3, glm::i32vec3> && std::is_same_v<glm::ivec4, glm::i32vec4>);
    static_assert(std::is_same_v<glm::uvec2, glm::u32vec2> && std::is_same_v<glm::uvec3, glm::u32vec3> && std::is_same_v<glm::uvec4, glm::u32vec4>);

    std::string attributeQaSource(const char* path)
    {
        std::ifstream file(path);
        if (!file) throw std::runtime_error(std::string("Missing QA shader: ") + path);
        return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    }

    // Local owners die before TearDown destroys the current context, including
    // on a fatal ASSERT. This adds no production VBO/lifetime behavior.
    struct AttributeQaProgram
    {
        GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
        GLuint program = glCreateProgram();

        AttributeQaProgram() = default;
        AttributeQaProgram(const AttributeQaProgram&) = delete;
        AttributeQaProgram& operator=(const AttributeQaProgram&) = delete;
        ~AttributeQaProgram()
        {
            if (program) glDeleteProgram(program);
            if (fragment) glDeleteShader(fragment);
            if (vertex) glDeleteShader(vertex);
        }

        bool compile(GLuint shader, const std::string& source)
        {
            const char* text = source.c_str();
            glShaderSource(shader, 1, &text, nullptr);
            glCompileShader(shader);
            GLint status = GL_FALSE, length = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::vector<GLchar> log(static_cast<std::size_t>(length > 0 ? length : 1), '\0');
            if (length > 0) glGetShaderInfoLog(shader, length, nullptr, log.data());
            std::cout << "[AttributeQA] compile status=" << status << " log=" << log.data() << '\n';
            return status == GL_TRUE;
        }

        bool load(const std::string& source, const std::vector<const char*>& varyings = {}, GLenum mode = GL_INTERLEAVED_ATTRIBS)
        {
            if (!compile(vertex, source)) return false;
            if (!compile(fragment, attributeQaSource("test-datas/shaders/qa-attribute/minimal.frag"))) return false;
            glAttachShader(program, vertex);
            glAttachShader(program, fragment);
            if (!varyings.empty()) glTransformFeedbackVaryings(program, static_cast<GLsizei>(varyings.size()), varyings.data(), mode);
            glLinkProgram(program);
            GLint status = GL_FALSE, length = 0;
            glGetProgramiv(program, GL_LINK_STATUS, &status);
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            std::vector<GLchar> log(static_cast<std::size_t>(length > 0 ? length : 1), '\0');
            if (length > 0) glGetProgramInfoLog(program, length, nullptr, log.data());
            std::cout << "[AttributeQA] link status=" << status << " log=" << log.data() << '\n';
            return status == GL_TRUE;
        }
    };
}

class AttributeQaPreflightTest : public OpenGLComponentTest
{
protected:
    void SetUp() override
    {
        OpenGLComponentTest::SetUp();
        const GLenum glewError = glGetError();
        std::cout << "[AttributeQA] one GLEW error read=" << glewError << '\n';
        ASSERT_TRUE(glewError == GL_NO_ERROR || glewError == GL_INVALID_ENUM);
        ASSERT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
        ASSERT_TRUE(GLEW_VERSION_3_3);
        ASSERT_NE(glVertexAttribIPointer, nullptr);
        ASSERT_NE(glTransformFeedbackVaryings, nullptr);
        ASSERT_NE(glBeginTransformFeedback, nullptr);
        ASSERT_NE(glEndTransformFeedback, nullptr);
        ASSERT_NE(glGetBufferSubData, nullptr);
        ASSERT_NE(glGenQueries, nullptr);
        ASSERT_EQ(glfwGetCurrentContext(), window);
    }
};

// NATIVE_API_PROBE only. Failure here is a precondition gap, never Setter RED.
TEST_F(AttributeQaPreflightTest, NativeCapabilitiesAndArraySpelling)
{
    GLint profile = 0, major = 0, minor = 0;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    std::cout << "[AttributeQA] requested=3.3core obtained=" << major << '.' << minor
              << " profile=" << profile << " GLSL=" << glGetString(GL_SHADING_LANGUAGE_VERSION)
              << " context=" << glfwGetCurrentContext() << " thread=" << std::this_thread::get_id()
              << " share-group=unshared fixture" << '\n';
    ASSERT_NE(profile & GL_CONTEXT_CORE_PROFILE_BIT, 0);
    const std::array<std::pair<GLenum, GLint>, 6> limits{{
        {GL_MAX_VERTEX_ATTRIBS, 16}, {GL_MAX_VERTEX_OUTPUT_COMPONENTS, 64},
        {GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS, 64},
        {GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, 4},
        {GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS, 4},
        {GL_MAX_VARYING_COMPONENTS, 34}}};
    for (const auto& [name, minimum] : limits)
    {
        GLint value = 0;
        glGetIntegerv(name, &value);
        std::cout << "[AttributeQA] limit " << name << '=' << value << " required=" << minimum << '\n';
        ASSERT_GE(value, minimum);
    }
    ASSERT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));

    for (const auto& [file, expectedName, expectedSize] :
         std::array<std::tuple<const char*, const char*, GLint>, 2>{{
             {"array-one.vert", "qaArray[0]", 1}, {"array-two.vert", "qaArray2[0]", 2}}})
    {
        SCOPED_TRACE(file);
        AttributeQaProgram program;
        const auto source = attributeQaSource((std::string("test-datas/shaders/qa-attribute/") + file).c_str());
        ASSERT_TRUE(program.load(source)) << "Q2 stop: legal GLSL330 array probe did not compile/link";
        GLint count = 0, maximum = 0;
        glGetProgramiv(program.program, GL_ACTIVE_ATTRIBUTES, &count);
        glGetProgramiv(program.program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maximum);
        std::cout << "[AttributeQA] " << file << " count=" << count << " MAX_LENGTH=" << maximum << '\n';
        ASSERT_GT(maximum, 0);
        ASSERT_EQ(count, 1);
        std::vector<GLchar> name(static_cast<std::size_t>(maximum), static_cast<GLchar>(0x7f));
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = 0;
        glGetActiveAttrib(program.program, 0, maximum, &length, &size, &type, name.data());
        ASSERT_GE(length, 0);
        ASSERT_LT(length, maximum);
        const auto location = glGetAttribLocation(program.program, name.data());
        const std::string exact(name.data(), static_cast<std::size_t>(length));
        std::cout << "[AttributeQA] " << file << " name='" << exact << "' length=" << length
                  << " size=" << size << " type=" << type << " location=" << location
                  << " terminator=" << static_cast<int>(name[static_cast<std::size_t>(length)]) << '\n';
        ASSERT_EQ(name[static_cast<std::size_t>(length)], '\0');
        ASSERT_EQ(exact, expectedName) << "Q2 stop: spelling not native [0], arbitration required";
        ASSERT_EQ(size, expectedSize);
        ASSERT_EQ(type, static_cast<GLenum>(GL_FLOAT_VEC3));
        ASSERT_GE(location, 0);
        ASSERT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
    }
}

TEST_F(AttributeQaPreflightTest, NativeInterfacesNamesAndRefusals)
{
    {
        AttributeQaProgram program;
        const std::vector<const char*> varyings{"b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8", "b9", "b10", "b11"};
        ASSERT_TRUE(program.load(attributeQaSource("test-datas/shaders/qa-attribute/identity-twelve.vert"), varyings));
        GLint count = 0;
        glGetProgramiv(program.program, GL_ACTIVE_ATTRIBUTES, &count);
        ASSERT_EQ(count, 12);
        for (GLint index = 0; index < 12; ++index)
        {
            const auto name = "a" + std::to_string(index);
            const auto location = glGetAttribLocation(program.program, name.c_str());
            std::cout << "[AttributeQA] twelve name=" << name << " location=" << location << '\n';
            ASSERT_EQ(location, index);
        }
    }
    {
        AttributeQaProgram program;
        ASSERT_TRUE(program.load(attributeQaSource("test-datas/shaders/qa-attribute/identity-separate.vert"), {"b0", "b4", "b8", "b3"}, GL_SEPARATE_ATTRIBS));
    }
    {
        AttributeQaProgram program;
        ASSERT_TRUE(program.load(attributeQaSource("test-datas/shaders/qa-attribute/matrix.vert")));
        ASSERT_EQ(glGetAttribLocation(program.program, "qaMatrix"), 1);
    }
    bool longNameLinked = false;
    for (const std::size_t length : {300u, 256u})
    {
        const std::string expected(length, 'a');
        auto source = attributeQaSource("test-datas/shaders/qa-attribute/long-name.vert");
        const std::string marker = "QA_LONG_ATTRIBUTE";
        for (auto at = source.find(marker); at != std::string::npos; at = source.find(marker, at + expected.size()))
            source.replace(at, marker.size(), expected);
        AttributeQaProgram program;
        std::cout << "[AttributeQA] natural identifier length=" << length << '\n';
        if (!program.load(source)) continue;
        GLint maximum = 0, count = 0;
        glGetProgramiv(program.program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maximum);
        glGetProgramiv(program.program, GL_ACTIVE_ATTRIBUTES, &count);
        ASSERT_EQ(count, 1);
        ASSERT_GE(maximum, static_cast<GLint>(length + 1));
        std::vector<GLchar> name(static_cast<std::size_t>(maximum), '\x7f');
        GLsizei observedLength = 0;
        GLint size = 0;
        GLenum type = 0;
        glGetActiveAttrib(program.program, 0, maximum, &observedLength, &size, &type, name.data());
        ASSERT_EQ(observedLength, static_cast<GLsizei>(length));
        ASSERT_EQ(std::string(name.data(), length), expected);
        ASSERT_EQ(name[length], '\0');
        ASSERT_EQ(size, 1);
        ASSERT_EQ(type, static_cast<GLenum>(GL_FLOAT_VEC3));
        ASSERT_EQ(glGetAttribLocation(program.program, name.data()), 0);
        std::cout << "[AttributeQA] long name exact='" << expected << "' length=" << observedLength
                  << " MAX_LENGTH=" << maximum << " size=" << size << " type=" << type << " location=0 NUL=0" << '\n';
        longNameLinked = true;
        break;
    }
    ASSERT_TRUE(longNameLinked) << "Q2 stop: neither natural identifier links";
    ASSERT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
}

TEST_F(AttributeQaPreflightTest, NativeReaderLongNameRegression)
{
    const std::string name(300, 'a');
    auto source = attributeQaSource("test-datas/shaders/qa-attribute/long-name.vert");
    const std::string marker = "QA_LONG_ATTRIBUTE";
    for (auto at = source.find(marker); at != std::string::npos; at = source.find(marker, at + name.size())) source.replace(at, marker.size(), name);
    AttributeQaProgram program;
    ASSERT_TRUE(program.load(source));
    api::OpenGL::PassContext context;
    context.setPassID(program.program);
    artist::graphic::opengl::pipeline::component::pass::OpenGLPassAttributeReader<artist::graphic::opengl::profile::Pass::Classic>::on(context);
    ASSERT_TRUE(context.getAttributes().contains(name));
    const auto& c = context.getAttribute(name)->getContext();
    EXPECT_EQ(c.getGLType(), static_cast<GLenum>(GL_FLOAT_VEC3)); EXPECT_EQ(c.getGLSize(), 1u);
    EXPECT_EQ(c.getAttributeID(), 0u); EXPECT_FALSE(c.isGLArray());
}

TEST_F(AttributeQaPreflightTest, NativeReaderArrayOneRegression)
{
    AttributeQaProgram program;
    ASSERT_TRUE(program.load(attributeQaSource("test-datas/shaders/qa-attribute/array-one.vert")));
    api::OpenGL::PassContext context;
    context.setPassID(program.program);
    artist::graphic::opengl::pipeline::component::pass::OpenGLPassAttributeReader<artist::graphic::opengl::profile::Pass::Classic>::on(context);
    ASSERT_TRUE(context.getAttributes().contains("qaArray[0]"));
    const auto& c = context.getAttribute("qaArray[0]")->getContext();
    EXPECT_EQ(c.getGLType(), static_cast<GLenum>(GL_FLOAT_VEC3)); EXPECT_EQ(c.getGLSize(), 1u);
    EXPECT_EQ(c.getAttributeID(), 0u); EXPECT_TRUE(c.isGLArray());
}

class AttributeQaRenderTest : public AttributeQaPreflightTest
{
protected:
    GLuint vao = 0;
    bool glReady = false;
    // Only decoys and transform-feedback outputs belong to the fixture.
    // Attribute VBO names are borrowed and retired by their own local owners.
    std::vector<GLuint> fixtureBuffers;
    std::vector<GLuint> queries;
    void SetUp() override
    {
        AttributeQaPreflightTest::SetUp();
        if (HasFatalFailure()) return;
        glReady = true;
        glGenVertexArrays(1, &vao); glBindVertexArray(vao);
    }
    GLuint buffer()
    {
        GLuint id = 0; glGenBuffers(1, &id); fixtureBuffers.push_back(id); return id;
    }
    GLuint decoy()
    {
        const auto id = buffer();
        const std::array<GLubyte, 16> bytes{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        glBindBuffer(GL_ARRAY_BUFFER, id); glBufferData(GL_ARRAY_BUFFER, bytes.size(), bytes.data(), GL_STATIC_DRAW);
        return id;
    }
    void verifyDecoy(GLuint id)
    {
        const std::array<GLubyte, 16> expected{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        glBindBuffer(GL_ARRAY_BUFFER, id);
        ASSERT_EQ(boundArrayBufferSize(), 16);
        std::array<GLubyte, 16> actual{};
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, actual.size(), actual.data());
        ASSERT_EQ(actual, expected);
    }
    template <typename T>
    void upload(api::OpenGL::PassContext& pass, GLint index, T value, GLenum shader, GLint width, GLenum scalar, bool integral, GLuint decoyId, std::vector<GLubyte>& expected)
    {
        SCOPED_TRACE("a" + std::to_string(index));
        auto* a = pass.getAttribute("a" + std::to_string(index));
        ASSERT_NE(a, nullptr);
        auto& c = a->getContext();
        ASSERT_EQ(c.getAttributeID(), static_cast<GLuint>(index));
        ASSERT_EQ(c.getGLType(), shader); ASSERT_EQ(c.getGLSize(), 1u); ASSERT_FALSE(c.isGLArray());
        ASSERT_EQ(c.getBufferID(), 0u); ASSERT_EQ(c.getGLUsage(), 0u);
        c.setGLUsage(GL_STATIC_DRAW); a->bind();
        const auto target = c.getBufferID();
        glBindBuffer(GL_ARRAY_BUFFER, decoyId);
        a->set(std::make_shared<T>(value));
        ASSERT_EQ(boundArrayBuffer(), static_cast<GLint>(target)); // Setter leaves the target bound.
        const auto query = [index](GLenum pname) {GLint result = -1; glGetVertexAttribiv(index, pname, &result); return result;};
        ASSERT_EQ(query(GL_VERTEX_ATTRIB_ARRAY_INTEGER), integral ? GL_TRUE : GL_FALSE);
        ASSERT_EQ(query(GL_VERTEX_ATTRIB_ARRAY_TYPE), static_cast<GLint>(scalar));
        ASSERT_EQ(query(GL_VERTEX_ATTRIB_ARRAY_SIZE), width);
        ASSERT_EQ(query(GL_VERTEX_ATTRIB_ARRAY_STRIDE), static_cast<GLint>(sizeof(T)));
        ASSERT_EQ(query(GL_VERTEX_ATTRIB_ARRAY_NORMALIZED), GL_FALSE);
        ASSERT_EQ(query(GL_VERTEX_ATTRIB_ARRAY_DIVISOR), 0);
        ASSERT_EQ(query(GL_VERTEX_ATTRIB_ARRAY_ENABLED), GL_FALSE); // Enable belongs to this fixture, not Setter.
        ASSERT_EQ(query(GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING), static_cast<GLint>(target));
        void* pointer = reinterpret_cast<void*>(1);
        glGetVertexAttribPointerv(index, GL_VERTEX_ATTRIB_ARRAY_POINTER, &pointer);
        ASSERT_EQ(pointer, nullptr);
        ASSERT_EQ(boundArrayBufferSize(), static_cast<GLint>(sizeof(T)));
        std::vector<GLubyte> bytes(sizeof(T));
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(T), bytes.data());
        std::vector<GLubyte> wanted(sizeof(T)); std::memcpy(wanted.data(), &value, sizeof(T));
        ASSERT_EQ(bytes, wanted);
        verifyDecoy(decoyId);
        ASSERT_EQ(query(GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING), static_cast<GLint>(target));
        glEnableVertexAttribArray(index);
        ASSERT_EQ(query(GL_VERTEX_ATTRIB_ARRAY_ENABLED), GL_TRUE);
        expected.insert(expected.end(), wanted.begin(), wanted.end());
        ASSERT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
        std::cout << "[AttributeQA] a" << index << " shader=" << shader << " scalar=" << scalar
                  << " width=" << width << " stride=" << sizeof(T) << " integer=" << integral
                  << " target=" << target << " payload exact, decoy intact" << '\n';
    }
    void capture(GLuint program, const std::vector<std::vector<GLubyte>>& expected, GLenum mode)
    {
        // Never enter feedback after ANY interface assertion, especially INTEGER/TYPE.
        ASSERT_FALSE(HasFailure()) << "No draw/TF with an invalid typed interface";
        GLint actualMode = 0;
        glGetProgramiv(program, GL_TRANSFORM_FEEDBACK_BUFFER_MODE, &actualMode);
        ASSERT_EQ(actualMode, static_cast<GLint>(mode));
        std::vector<GLuint> outputs;
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            const auto id = buffer(); outputs.push_back(id);
            const std::vector<GLubyte> initial(expected[i].size() + 16, 0xa5);
            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, id);
            glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, initial.size(), initial.data(), GL_STATIC_READ);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, static_cast<GLuint>(i), id);
        }
        GLuint query = 0; glGenQueries(1, &query); queries.push_back(query);
        glUseProgram(program); glEnable(GL_RASTERIZER_DISCARD);
        ASSERT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
        glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
        glBeginTransformFeedback(GL_POINTS);
        glDrawArrays(GL_POINTS, 0, 1);
        glEndTransformFeedback();
        glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
        glDisable(GL_RASTERIZER_DISCARD);
        ASSERT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
        GLuint written = 0; glGetQueryObjectuiv(query, GL_QUERY_RESULT, &written);
        ASSERT_EQ(written, 1u);
        for (std::size_t i = 0; i < outputs.size(); ++i)
        {
            SCOPED_TRACE(i);
            std::vector<GLubyte> actual(expected[i].size() + 16);
            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, outputs[i]);
            // This ordered readback waits for the corresponding GPU writes; no glFinish/barrier is needed.
            glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, actual.size(), actual.data());
            ASSERT_EQ(std::vector<GLubyte>(actual.begin(), actual.begin() + expected[i].size()), expected[i]);
            ASSERT_EQ(std::vector<GLubyte>(actual.begin() + expected[i].size(), actual.end()), std::vector<GLubyte>(16, 0xa5));
            if (const char* directory = std::getenv("ARTIST_ATTRIBUTE_QA_EVIDENCE_DIR"))
            {
                const auto prefix = std::string(directory) + (mode == GL_INTERLEAVED_ATTRIBS ? "/interleaved-" : "/separate-") + std::to_string(i);
                std::ofstream readback(prefix + "-readback.bin", std::ios::binary);
                std::ofstream reference(prefix + "-expected.bin", std::ios::binary);
                readback.write(reinterpret_cast<const char*>(actual.data()), static_cast<std::streamsize>(actual.size()));
                reference.write(reinterpret_cast<const char*>(expected[i].data()), static_cast<std::streamsize>(expected[i].size()));
                ASSERT_TRUE(readback.good()); ASSERT_TRUE(reference.good());
            }
            std::cout << "[AttributeQA] TF mode=" << mode << " buffer=" << i << " exact bytes=" << expected[i].size()
                      << " sentinel16=0xA5 query=1" << '\n';
        }
        ASSERT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
    }
    void TearDown() override
    {
        if (glReady)
        {
            glUseProgram(0); glDisable(GL_RASTERIZER_DISCARD);
            if (!queries.empty()) glDeleteQueries(static_cast<GLsizei>(queries.size()), queries.data());
            if (!fixtureBuffers.empty()) glDeleteBuffers(static_cast<GLsizei>(fixtureBuffers.size()), fixtureBuffers.data());
            if (vao) glDeleteVertexArrays(1, &vao);
        }
        AttributeQaPreflightTest::TearDown();
    }
};

TEST_F(AttributeQaRenderTest, NativeTargetBindingRegression)
{
    Attribute a(makeContext(1, GL_FLOAT)); a.bind();
    const auto target = a.getContext().getBufferID();
    const auto decoy = buffer();
    const std::array<unsigned char, 16> original{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    glBindBuffer(GL_ARRAY_BUFFER, decoy); glBufferData(GL_ARRAY_BUFFER, original.size(), original.data(), GL_STATIC_DRAW);
    a.set(std::make_shared<float>(0.5f));
    EXPECT_EQ(boundArrayBuffer(), static_cast<GLint>(target));
    glBindBuffer(GL_ARRAY_BUFFER, decoy);
    ASSERT_EQ(boundArrayBufferSize(), static_cast<GLint>(original.size()));
    std::array<unsigned char, 16> actual{}; glGetBufferSubData(GL_ARRAY_BUFFER, 0, actual.size(), actual.data());
    EXPECT_EQ(actual, original);
    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
}

TEST_F(AttributeQaRenderTest, NativeIntegerInterfaceRegression)
{
    Attribute a(makeContext(1, GL_INT)); a.bind(); a.set(std::make_shared<std::int32_t>(16777217));
    GLint integer = -1, type = 0;
    glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_INTEGER, &integer);
    glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type);
    ASSERT_EQ(integer, GL_TRUE) << "No draw/TF is permitted after a mismatched integer interface";
    ASSERT_EQ(type, GL_INT);
    ASSERT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
}

TEST_F(AttributeQaRenderTest, NativeVec3MappingRegression)
{
    Attribute a(makeContext(1, GL_FLOAT_VEC3)); a.bind(); a.set(std::make_shared<glm::vec3>(1, 2, 3));
    GLint size = 0, type = 0;
    glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
    glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type);
    EXPECT_EQ(size, 3); EXPECT_EQ(type, GL_FLOAT);
    EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
}

TEST_F(AttributeQaRenderTest, TwelveReflectedFormsConsumeExactTypedIdentityInterleaved)
{
    RecordProperty("evidence_class", "NATIVE_READER_STATE_TF_INTERLEAVED");
    AttributeQaProgram program;
    ASSERT_TRUE(program.load(attributeQaSource("test-datas/shaders/qa-attribute/identity-twelve.vert"), {"b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8", "b9", "b10", "b11"}));
    api::OpenGL::PassContext pass; pass.setPassID(program.program);
    artist::graphic::opengl::pipeline::component::pass::OpenGLPassAttributeReader<artist::graphic::opengl::profile::Pass::Classic>::on(pass);
    ASSERT_EQ(pass.getAttributes().size(), 12u);
    const auto lure = decoy();
    std::vector<GLubyte> expected;
    upload(pass, 0, 0.5f, GL_FLOAT, 1, GL_FLOAT, false, lure, expected);
    upload(pass, 1, glm::vec2(16777216.0f, 16777218.0f), GL_FLOAT_VEC2, 2, GL_FLOAT, false, lure, expected);
    upload(pass, 2, glm::vec3(-1.0f, 0.25f, 0.75f), GL_FLOAT_VEC3, 3, GL_FLOAT, false, lure, expected);
    upload(pass, 3, glm::vec4(1.0f, -2.0f, 0.5f, 16777216.0f), GL_FLOAT_VEC4, 4, GL_FLOAT, false, lure, expected);
    constexpr auto min = std::numeric_limits<std::int32_t>::min(), max = std::numeric_limits<std::int32_t>::max();
    constexpr auto umax = std::numeric_limits<std::uint32_t>::max();
    upload(pass, 4, std::int32_t(16777217), GL_INT, 1, GL_INT, true, lure, expected);
    upload(pass, 5, glm::ivec2(min, max), GL_INT_VEC2, 2, GL_INT, true, lure, expected);
    upload(pass, 6, glm::ivec3(-1, 16777217, 16777216), GL_INT_VEC3, 3, GL_INT, true, lure, expected);
    upload(pass, 7, glm::ivec4(min, -16777217, 16777217, max), GL_INT_VEC4, 4, GL_INT, true, lure, expected);
    upload(pass, 8, std::uint32_t(umax), GL_UNSIGNED_INT, 1, GL_UNSIGNED_INT, true, lure, expected);
    upload(pass, 9, glm::uvec2(0u, 16777217u), GL_UNSIGNED_INT_VEC2, 2, GL_UNSIGNED_INT, true, lure, expected);
    upload(pass, 10, glm::uvec3(1u, 16777216u, 16777217u), GL_UNSIGNED_INT_VEC3, 3, GL_UNSIGNED_INT, true, lure, expected);
    upload(pass, 11, glm::uvec4(umax, 16777217u, 2u, 2147483648u), GL_UNSIGNED_INT_VEC4, 4, GL_UNSIGNED_INT, true, lure, expected);
    ASSERT_FALSE(HasFailure()); ASSERT_EQ(expected.size(), 120u);
    capture(program.program, {expected}, GL_INTERLEAVED_ATTRIBS);
}

TEST_F(AttributeQaRenderTest, FourSeparateBuffersConsumeExactFloatIntUintVec4)
{
    RecordProperty("evidence_class", "NATIVE_TF_SEPARATE");
    AttributeQaProgram program;
    ASSERT_TRUE(program.load(attributeQaSource("test-datas/shaders/qa-attribute/identity-separate.vert"), {"b0", "b4", "b8", "b3"}, GL_SEPARATE_ATTRIBS));
    api::OpenGL::PassContext pass; pass.setPassID(program.program);
    artist::graphic::opengl::pipeline::component::pass::OpenGLPassAttributeReader<artist::graphic::opengl::profile::Pass::Classic>::on(pass);
    ASSERT_EQ(pass.getAttributes().size(), 4u);
    const auto lure = decoy();
    std::vector<std::vector<GLubyte>> expected(4);
    upload(pass, 0, 0.5f, GL_FLOAT, 1, GL_FLOAT, false, lure, expected[0]);
    upload(pass, 4, std::int32_t(16777217), GL_INT, 1, GL_INT, true, lure, expected[1]);
    upload(pass, 8, std::numeric_limits<std::uint32_t>::max(), GL_UNSIGNED_INT, 1, GL_UNSIGNED_INT, true, lure, expected[2]);
    upload(pass, 3, glm::vec4(1.0f, -2.0f, 0.5f, 16777216.0f), GL_FLOAT_VEC4, 4, GL_FLOAT, false, lure, expected[3]);
    ASSERT_FALSE(HasFailure());
    capture(program.program, expected, GL_SEPARATE_ATTRIBS);
}

TEST_F(AttributeQaRenderTest, NativeArraysDescribeThenRefuseWithoutTouchingDecoy)
{
    for (const auto& [file, name, size] : std::array<std::tuple<const char*, const char*, GLuint>, 2>{{
        {"array-one.vert", "qaArray[0]", 1}, {"array-two.vert", "qaArray2[0]", 2}}})
    {
        SCOPED_TRACE(file);
        AttributeQaProgram program;
        ASSERT_TRUE(program.load(attributeQaSource((std::string("test-datas/shaders/qa-attribute/") + file).c_str())));
        api::OpenGL::PassContext pass; pass.setPassID(program.program);
        artist::graphic::opengl::pipeline::component::pass::OpenGLPassAttributeReader<artist::graphic::opengl::profile::Pass::Classic>::on(pass);
        auto* a = pass.getAttribute(name); ASSERT_NE(a, nullptr);
        ASSERT_TRUE(a->getContext().isGLArray()); ASSERT_EQ(a->getContext().getGLSize(), size);
        ASSERT_EQ(a->getContext().getGLType(), static_cast<GLenum>(GL_FLOAT_VEC3));
        a->getContext().setGLUsage(GL_STATIC_DRAW); a->bind();
        const auto lure = decoy();
        expectSpecificError([&] { a->set(std::make_shared<glm::vec3>(1)); }, TraceableException<std::runtime_error>(std::format("ERROR::ATTRIBUTE::SET::ARRAY_NOT_SUPPORTED size={}", size)));
        EXPECT_EQ(boundArrayBuffer(), static_cast<GLint>(lure)); verifyDecoy(lure);
        ASSERT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
    }
}

TEST_F(AttributeQaRenderTest, MatrixPublishedThenSetterRefuses)
{
    AttributeQaProgram program;
    ASSERT_TRUE(program.load(attributeQaSource("test-datas/shaders/qa-attribute/matrix.vert")));
    for (bool unsupported : {false, true})
    {
        api::OpenGL::PassContext pass; pass.setPassID(program.program);
        artist::graphic::opengl::pipeline::component::pass::OpenGLPassAttributeReader<artist::graphic::opengl::profile::Pass::Classic>::on(pass);
        auto* a = pass.getAttribute("qaMatrix"); ASSERT_NE(a, nullptr);
        ASSERT_EQ(a->getContext().getGLType(), static_cast<GLenum>(GL_FLOAT_MAT4)); ASSERT_EQ(a->getContext().getGLSize(), 1u);
        ASSERT_FALSE(a->getContext().isGLArray());
        a->getContext().setGLUsage(GL_STATIC_DRAW); a->bind();
        const auto lure = decoy();
        if (unsupported)
            expectSpecificError([&] { a->set(std::make_shared<glm::mat4>(1)); }, TraceableException<std::runtime_error>("ERROR::ATTRIBUTE::UNSUPPORTED_TYPE"));
        else
            expectSpecificError([&] { a->set(std::make_shared<glm::vec4>(1)); }, TraceableException<std::runtime_error>(std::format("ERROR::ATTRIBUTE::SET::SHADER_TYPE_MISMATCH expected={} actual={}", static_cast<unsigned>(GL_FLOAT_VEC4), static_cast<unsigned>(GL_FLOAT_MAT4))));
        EXPECT_EQ(boundArrayBuffer(), static_cast<GLint>(lure)); verifyDecoy(lure);
        ASSERT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
    }
}

TEST_F(AttributeQaRenderTest, SameTypeSecondPayloadAndDifferentTypeFreeze)
{
    Attribute a(makeContext(1, GL_FLOAT)); a.bind();
    a.set(std::make_shared<float>(0.5f));
    auto second = std::make_shared<float>(1.5f); a.set(second);
    float value = 0; glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(value), &value);
    ASSERT_EQ(value, 1.5f); EXPECT_EQ(a.getContext().getValue<float>(), second);
    const auto target = a.getContext().getBufferID(), lure = decoy();
    expectSpecificError([&] { a.set(std::make_shared<std::int32_t>(7)); }, TraceableException<std::runtime_error>(std::format("ERROR::ATTRIBUTE::SET::TYPE_MISMATCH: The type of the value ({}) does not match the type of the attribute variable ({})", typeid(std::shared_ptr<std::int32_t>).name(), typeid(std::shared_ptr<float>).name())));
    EXPECT_EQ(boundArrayBuffer(), static_cast<GLint>(lure)); verifyDecoy(lure);
    EXPECT_EQ(a.getContext().getValue<float>(), second);
    glBindBuffer(GL_ARRAY_BUFFER, target); glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(value), &value);
    EXPECT_EQ(value, 1.5f); EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
}
