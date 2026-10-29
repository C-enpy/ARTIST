#include <gtest/gtest.h>
#include <array>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <graphic/pipeline/Pass.hpp>
#include <graphic/opengl/profile/Pass.hpp>
#include <graphic/opengl/profile/Shader.hpp>
#include <graphic/opengl/validator/Validator.hpp>
#include <graphic/opengl/pipeline/component/attribute/Binder.hpp>
#include <graphic/opengl/pipeline/component/attribute/Setter.hpp>
#include <graphic/opengl/pipeline/component/attribute/Unbinder.hpp>
#include <graphic/opengl/pipeline/component/uniform/Setter.hpp>
#include <graphic/opengl/pipeline/component/shader/Freer.hpp>
#include <graphic/opengl/pipeline/component/shader/Loader.hpp>
#include <graphic/opengl/pipeline/component/shader/Reader.hpp>
#include <graphic/opengl/pipeline/component/pass/ShaderAttacher.hpp>
#include <graphic/opengl/pipeline/component/pass/UniformReader.hpp>
#include <graphic/opengl/pipeline/component/pass/AttributeReader.hpp>
#include <graphic/opengl/pipeline/component/pass/Loader.hpp>
#include <graphic/opengl/pipeline/component/pass/User.hpp>
#include <graphic/opengl/pipeline/component/pass/Freer.hpp>
#include <graphic/opengl/pipeline/PreparedFloatUniform.hpp>
#include <OpenGLComponentTests.hpp>

namespace
{
    using API = artist::graphic::api::OpenGL;
    namespace pipeline = artist::graphic::pipeline;
    namespace profile = artist::graphic::opengl::profile;
    using Pass = pipeline::Pass<API, profile::Pass::Classic>;
    using artist::graphic::opengl::pipeline::prepareFloatUniform;
    constexpr GLsizei extent = 32;
    using Pixel = std::array<GLubyte, 4>;
    using Image = std::array<GLubyte, extent * extent * 4>;
    constexpr Pixel white{255, 255, 255, 255}, background{0, 0, 0, 255};

    Pass loadedPass()
    {
        std::vector<std::unique_ptr<pipeline::IShader<API>>> shaders;
        shaders.push_back(std::make_unique<pipeline::Shader<API, profile::Shader::Classic>>(
            "test-datas/shaders/vertex/good/f2-single-values.vert",
            artist::graphic::context::ShaderType::VERTEX));
        shaders.push_back(std::make_unique<pipeline::Shader<API, profile::Shader::Classic>>(
            "test-datas/shaders/fragment/good/minimal.frag",
            artist::graphic::context::ShaderType::FRAGMENT));
        Pass pass(std::move(shaders));
        pass.load();
        return pass;
    }

    struct ContextWitness
    {
        GLFWwindow *window;
        ~ContextWitness() { EXPECT_EQ(glfwGetCurrentContext(), window); }
    };

    struct ProgramBinding
    {
        explicit ProgramBinding(Pass &pass) { pass.use(); }
        ~ProgramBinding()
        {
            glUseProgram(0);
            EXPECT_EQ(glGetError(), GL_NO_ERROR);
        }
    };

    // Test-local resources, retired before the program and the fixture's window.
    // The fixture owns a fresh, unshared context, not an application's GL state.
    struct TriangleTarget
    {
        GLuint framebuffer = 0, color = 0, vao = 0, vbo = 0;
        TriangleTarget()
        {
            glGenFramebuffers(1, &framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glGenRenderbuffers(1, &color);
            glBindRenderbuffer(GL_RENDERBUFFER, color);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, extent, extent);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glReadBuffer(GL_COLOR_ATTACHMENT0);

            constexpr std::array<GLfloat, 9> vertices{
                -.75f, -.75f, 0.f, .75f, -.75f, 0.f, 0.f, .75f, 0.f};
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
            glEnableVertexAttribArray(0);

            glViewport(0, 0, extent, extent);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            for (GLenum state : {GL_BLEND, GL_DITHER, GL_FRAMEBUFFER_SRGB, GL_MULTISAMPLE,
                GL_CULL_FACE, GL_SCISSOR_TEST, GL_DEPTH_TEST, GL_STENCIL_TEST,
                GL_RASTERIZER_DISCARD, GL_POLYGON_SMOOTH, GL_COLOR_LOGIC_OP,
                GL_SAMPLE_COVERAGE, GL_SAMPLE_ALPHA_TO_COVERAGE})
                glDisable(state);
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            glPixelStorei(GL_PACK_ROW_LENGTH, 0);
            glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
            glPixelStorei(GL_PACK_SKIP_ROWS, 0);
        }
        TriangleTarget(const TriangleTarget &) = delete;
        TriangleTarget &operator=(const TriangleTarget &) = delete;
        ~TriangleTarget()
        {
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteBuffers(1, &vbo);
            glDeleteVertexArrays(1, &vao);
            glDeleteRenderbuffers(1, &color);
            glDeleteFramebuffers(1, &framebuffer);
            EXPECT_EQ(glGetError(), GL_NO_ERROR);
        }
    };

    Pixel pixelAt(const Image &image, int x, int y)
    {
        const auto offset = static_cast<std::size_t>((y * extent + x) * 4);
        return {image[offset], image[offset + 1], image[offset + 2], image[offset + 3]};
    }

    void expectOracle(const Image &image, bool shifted, const char *route)
    {
        SCOPED_TRACE(::testing::Message() << route << " scalar=" << (shifted ? .5f : 0.f));
        // Bottom-left origin, sample centers (x+.5,y+.5), far from all edges.
        EXPECT_EQ(pixelAt(image, 8, 8), shifted ? background : white);
        EXPECT_EQ(pixelAt(image, 24, 24), shifted ? white : background);
        EXPECT_EQ(pixelAt(image, 18, 16), white); // Both routes must actually draw.
        EXPECT_EQ(pixelAt(image, 2, 2), background);
        std::cout << "[pixels] " << route << " scalar=" << (shifted ? .5f : 0.f);
        for (const auto &xy : {std::array{8, 8}, std::array{24, 24},
            std::array{18, 16}, std::array{2, 2}})
        {
            const auto pixel = pixelAt(image, xy[0], xy[1]);
            std::cout << " (" << xy[0] << ',' << xy[1] << ")=";
            for (auto channel : pixel) std::cout << unsigned(channel) << ',';
        }
        std::cout << '\n';
    }

    // A serialized, pass-through GLEW spy, not a replacement driver. Its lifetime
    // is entirely inside one test under the mission's exclusive GPU lease.
    class LocationSpy
    {
    public:
        LocationSpy() : original(__glewGetUniformLocation)
        {
            EXPECT_EQ(active, nullptr);
            active = this;
            __glewGetUniformLocation = &lookup;
        }
        ~LocationSpy()
        {
            __glewGetUniformLocation = original;
            active = nullptr;
        }
        LocationSpy(const LocationSpy &) = delete;
        LocationSpy &operator=(const LocationSpy &) = delete;
        unsigned calls = 0;
    private:
        static GLint GLAPIENTRY lookup(GLuint program, const GLchar *name)
        {
            ++active->calls;
            return active->original(program, name);
        }
        PFNGLGETUNIFORMLOCATIONPROC original;
        static inline LocationSpy *active = nullptr;
    };
}

class PreparedFloatUniformRenderTest : public OpenGLComponentTest
{
protected:
    void SetUp() override
    {
        OpenGLComponentTest::SetUp();
        while (glGetError() != GL_NO_ERROR) {} // Only GLEW initialization, before the experiment.
        ASSERT_EQ(glfwGetCurrentContext(), window);
        GLint major = 0, minor = 0, profileMask = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profileMask);
        std::cout << "[prepared-render] requested=3.3 core obtained=" << major << '.' << minor
            << " profile=" << profileMask << " GLSL=" << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << " current=" << glfwGetCurrentContext() << " thread=" << std::this_thread::get_id()
            << " shareGroup=unshared(GLFW share=null)\n";
        ASSERT_GE(major * 10 + minor, 33);
        ASSERT_NE(profileMask & GL_CONTEXT_CORE_PROFILE_BIT, 0);
        ASSERT_EQ(glGetError(), GL_NO_ERROR);
    }
};

TEST_F(PreparedFloatUniformRenderTest, LegacyAndPreparedMatchFixedNonemptyPixels)
{
    ContextWitness witness{window};
    auto pass = loadedPass();
    ProgramBinding binding(pass);
    TriangleTarget target;
    ASSERT_EQ(glCheckFramebufferStatus(GL_FRAMEBUFFER), GL_FRAMEBUFFER_COMPLETE);
    GLint encoding = 0, samples = -1, format = 0;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING, &encoding);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &samples);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &format);
    ASSERT_EQ(encoding, GL_LINEAR);
    ASSERT_EQ(samples, 0);
    ASSERT_EQ(format, GL_RGBA8);

    pass.withUniform("f2Matrix", glm::mat3(1.f));
    pass.withUniform("f2Vector", glm::vec3(0.f));
    pass.withUniform("testUniform", GLint(1));
    const GLuint program = pass.getContext().getPassID();
    const GLint arrayLocation = glGetUniformLocation(program, "f2Array[0]");
    const GLint scalarLocation = glGetUniformLocation(program, "f2Scalar");
    ASSERT_GE(arrayLocation, 0);
    ASSERT_GE(scalarLocation, 0);
    constexpr std::array<GLfloat, 6> zeros{};
    glUniform3fv(arrayLocation, 2, zeros.data()); // Setup only; legacy array setter stays unchanged.
    const auto prepared = prepareFloatUniform(pass, "f2Scalar"); // Once, after runtime link/reflection.
    const auto &mirror = *pass.getUniforms().at("f2Scalar");
    ASSERT_EQ(glGetError(), GL_NO_ERROR) << "setup";

    std::array<Image, 2> legacy{}, typed{};
    auto renderSequence = [&](bool usePrepared, std::array<Image, 2> &images)
    {
        for (std::size_t frame = 0; frame < images.size(); ++frame)
        {
            const GLfloat value = frame == 0 ? 0.f : .5f;
            if (usePrepared) prepared.set(value);
            else pass.withUniform("f2Scalar", value);
            ASSERT_EQ(glGetError(), GL_NO_ERROR) << "update";
            glClear(GL_COLOR_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            ASSERT_EQ(glGetError(), GL_NO_ERROR) << "draw";
            glReadPixels(0, 0, extent, extent, GL_RGBA, GL_UNSIGNED_BYTE, images[frame].data());
            ASSERT_EQ(glGetError(), GL_NO_ERROR) << "read";
            expectOracle(images[frame], frame != 0, usePrepared ? "prepared" : "legacy");
            GLfloat actual = -1.f;
            glGetUniformfv(program, scalarLocation, &actual);
            EXPECT_EQ(actual, value);
            EXPECT_EQ(mirror.get<GLfloat>(), value);
            ASSERT_EQ(glGetError(), GL_NO_ERROR) << "uniform readback";
        }
    };
    renderSequence(false, legacy);
    ASSERT_FALSE(::testing::Test::HasFatalFailure());
    renderSequence(true, typed);
    ASSERT_FALSE(::testing::Test::HasFatalFailure());
    // Equality supplements (never replaces) each route's independent oracle.
    EXPECT_EQ(legacy, typed);
    EXPECT_NE(legacy[0], legacy[1]);
    EXPECT_NE(typed[0], typed[1]);
}

TEST_F(PreparedFloatUniformRenderTest, PreparedUpdatesDoNotResolveNativeLocationsAgain)
{
    ContextWitness witness{window};
    LocationSpy locations;
    auto pass = loadedPass();
    ProgramBinding binding(pass);
    const auto afterSetup = locations.calls;
    const GLuint program = pass.getContext().getPassID();
    const auto beforeCalibration = locations.calls;
    const GLint location = glGetUniformLocation(program, "f2Scalar");
    ASSERT_GE(location, 0);
    ASSERT_EQ(locations.calls, beforeCalibration + 1); // Discriminant calibration.
    const auto beforePreparation = locations.calls;
    const auto prepared = prepareFloatUniform(pass, "f2Scalar");
    const auto afterPreparation = locations.calls;
    for (float value : {0.f, .5f, -.25f}) prepared.set(value);
    const auto afterUpdates = locations.calls;
    EXPECT_EQ(afterUpdates, afterPreparation);
    ASSERT_EQ(glGetError(), GL_NO_ERROR) << "prepared update interval";

    // All CPU/name and native readbacks are after the observed interval.
    GLfloat actual = 0.f;
    glGetUniformfv(program, location, &actual);
    EXPECT_EQ(actual, -.25f);
    EXPECT_EQ(pass.getUniforms().at("f2Scalar")->get<GLfloat>(), -.25f);
    EXPECT_EQ(glGetError(), GL_NO_ERROR);
    std::cout << "[locations] setup=" << afterSetup << " calibration=1 preparation="
        << afterPreparation - beforePreparation << " updates=" << afterUpdates - afterPreparation
        << " uploads-requested=3; CPU-name/type-erased counters belong to the unit tests\n";
}
