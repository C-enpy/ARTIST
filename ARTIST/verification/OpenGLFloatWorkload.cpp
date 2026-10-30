// SOURCE ONLY in the delivery mission: no compilation/render/benchmark executed.
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "WorkloadProtocol.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <thread>
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

#ifndef ARTIST_VERIFICATION_SOURCE_ROOT
#error ARTIST_VERIFICATION_SOURCE_ROOT must identify the source fixtures, not the working directory
#endif

namespace
{
    namespace verification = artist::verification;
    namespace pipeline = artist::graphic::pipeline;
    namespace profile = artist::graphic::opengl::profile;
    using API = artist::graphic::api::OpenGL;
    using Pass = pipeline::Pass<API, profile::Pass::Classic>;
    using verification::require;
    constexpr std::array phaseValues{0.f, .5f, 0.f};
    constexpr GLsizei cellExtent = 32;
    using Pixel = std::array<GLubyte, 4>;
    constexpr Pixel white{255, 255, 255, 255}, background{0, 0, 0, 255};

    void checkGL(const char *stage)
    {
        const GLenum error = glGetError();
        require(error == GL_NO_ERROR, std::string(stage) + ": GL error " + std::to_string(error), 30);
    }

    struct NativeContext
    {
        GLFWwindow *window = nullptr;
        NativeContext()
        {
            require(glfwInit() == GLFW_TRUE, "GLFW initialization unavailable", 20);
            try
            {
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
                glfwWindowHint(GLFW_SAMPLES, 0);
                window = glfwCreateWindow(cellExtent, cellExtent, "ARTIST uniform verification", nullptr, nullptr);
                require(window != nullptr, "GLFW 3.3 core context unavailable", 20);
                glfwMakeContextCurrent(window);
                require(glfwGetCurrentContext() == window, "context not current on owner thread", 20);
                glewExperimental = GL_TRUE;
                require(glewInit() == GLEW_OK, "GLEW initialization unavailable", 20);
                require(GLEW_VERSION_3_3, "GLEW did not expose required OpenGL 3.3 entrypoints", 20);
#define ARTIST_VERIFY_GL_ENTRY(name) require((name) != nullptr, #name " entrypoint unavailable", 20)
                ARTIST_VERIFY_GL_ENTRY(glCreateShader);
                ARTIST_VERIFY_GL_ENTRY(glShaderSource);
                ARTIST_VERIFY_GL_ENTRY(glCompileShader);
                ARTIST_VERIFY_GL_ENTRY(glGetShaderiv);
                ARTIST_VERIFY_GL_ENTRY(glGetShaderInfoLog);
                ARTIST_VERIFY_GL_ENTRY(glCreateProgram);
                ARTIST_VERIFY_GL_ENTRY(glAttachShader);
                ARTIST_VERIFY_GL_ENTRY(glLinkProgram);
                ARTIST_VERIFY_GL_ENTRY(glGetProgramiv);
                ARTIST_VERIFY_GL_ENTRY(glGetProgramInfoLog);
                ARTIST_VERIFY_GL_ENTRY(glGetActiveUniform);
                ARTIST_VERIFY_GL_ENTRY(glGetUniformLocation);
                ARTIST_VERIFY_GL_ENTRY(glGetActiveAttrib);
                ARTIST_VERIFY_GL_ENTRY(glGetAttribLocation);
                ARTIST_VERIFY_GL_ENTRY(glUseProgram);
                ARTIST_VERIFY_GL_ENTRY(glDetachShader);
                ARTIST_VERIFY_GL_ENTRY(glDeleteShader);
                ARTIST_VERIFY_GL_ENTRY(glDeleteProgram);
                ARTIST_VERIFY_GL_ENTRY(glUniform1f);
                ARTIST_VERIFY_GL_ENTRY(glUniform1i);
                ARTIST_VERIFY_GL_ENTRY(glUniform3f);
                ARTIST_VERIFY_GL_ENTRY(glUniform3fv);
                ARTIST_VERIFY_GL_ENTRY(glUniformMatrix3fv);
                ARTIST_VERIFY_GL_ENTRY(glGetUniformfv);
                ARTIST_VERIFY_GL_ENTRY(glGenFramebuffers);
                ARTIST_VERIFY_GL_ENTRY(glBindFramebuffer);
                ARTIST_VERIFY_GL_ENTRY(glFramebufferRenderbuffer);
                ARTIST_VERIFY_GL_ENTRY(glCheckFramebufferStatus);
                ARTIST_VERIFY_GL_ENTRY(glGetFramebufferAttachmentParameteriv);
                ARTIST_VERIFY_GL_ENTRY(glDeleteFramebuffers);
                ARTIST_VERIFY_GL_ENTRY(glGenRenderbuffers);
                ARTIST_VERIFY_GL_ENTRY(glBindRenderbuffer);
                ARTIST_VERIFY_GL_ENTRY(glRenderbufferStorage);
                ARTIST_VERIFY_GL_ENTRY(glGetRenderbufferParameteriv);
                ARTIST_VERIFY_GL_ENTRY(glDeleteRenderbuffers);
                ARTIST_VERIFY_GL_ENTRY(glGenVertexArrays);
                ARTIST_VERIFY_GL_ENTRY(glBindVertexArray);
                ARTIST_VERIFY_GL_ENTRY(glDeleteVertexArrays);
                ARTIST_VERIFY_GL_ENTRY(glGenBuffers);
                ARTIST_VERIFY_GL_ENTRY(glBindBuffer);
                ARTIST_VERIFY_GL_ENTRY(glBufferData);
                ARTIST_VERIFY_GL_ENTRY(glDeleteBuffers);
                ARTIST_VERIFY_GL_ENTRY(glVertexAttribPointer);
                ARTIST_VERIFY_GL_ENTRY(glEnableVertexAttribArray);
#undef ARTIST_VERIFY_GL_ENTRY
                // GLEW may leave an error during bootstrap. Never drain errors
                // from the workload; this is before the experiment only.
                for (int attempt = 0; attempt < 16; ++attempt)
                    if (glGetError() == GL_NO_ERROR) break;
                checkGL("GLEW bootstrap error drain");
                GLint major = 0, minor = 0, mask = 0;
                glGetIntegerv(GL_MAJOR_VERSION, &major);
                glGetIntegerv(GL_MINOR_VERSION, &minor);
                glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask);
                require((major > 3 || (major == 3 && minor >= 3)) && mask == GL_CONTEXT_CORE_PROFILE_BIT,
                    "obtained context does not satisfy 3.3 core", 20);
                checkGL("context qualification");
            }
            catch (...)
            {
                if (window) { glfwMakeContextCurrent(nullptr); glfwDestroyWindow(window); }
                glfwTerminate();
                throw;
            }
        }
        NativeContext(const NativeContext &) = delete;
        NativeContext &operator=(const NativeContext &) = delete;
        ~NativeContext()
        {
            // All resources and Pass owners live in a nested scope and retire
            // first, while this unshared context is still current.
            glfwMakeContextCurrent(nullptr);
            glfwDestroyWindow(window);
            glfwTerminate();
        }
    };

    std::string nativeString(GLenum name)
    {
        const auto *value = glGetString(name);
        require(value != nullptr, "missing GL identity string", 20);
        return reinterpret_cast<const char *>(value);
    }

    std::string lowerAscii(std::string value)
    {
        for (char &letter : value)
            if (letter >= 'A' && letter <= 'Z') letter = static_cast<char>(letter - 'A' + 'a');
        return value;
    }

    void qualifyRenderer(const verification::Request &request, verification::Result &result, GLFWwindow *expectedContext)
    {
        require(expectedContext && glfwGetCurrentContext() == expectedContext,
            "owned GL context is not current on its owner thread", 20);
        const auto vendor = nativeString(GL_VENDOR), renderer = nativeString(GL_RENDERER);
        const auto version = nativeString(GL_VERSION), glsl = nativeString(GL_SHADING_LANGUAGE_VERSION);
        GLint major = 0, minor = 0, mask = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask);
        std::ostringstream thread;
        thread << std::this_thread::get_id();
        std::ostringstream current;
        current << static_cast<void *>(glfwGetCurrentContext());
        const auto lowered = lowerAscii(renderer);
        const bool llvmpipe = lowered.find("llvmpipe") != std::string::npos;
        const bool software = llvmpipe || lowered.find("softpipe") != std::string::npos
            || lowered.find("software") != std::string::npos || lowered.find("swiftshader") != std::string::npos
            || lowered.find("swrast") != std::string::npos || lowered.find("gdi generic") != std::string::npos;
        const bool contextOK = glfwGetCurrentContext() == expectedContext
            && (major > 3 || (major == 3 && minor >= 3)) && mask == GL_CONTEXT_CORE_PROFILE_BIT;
        bool fileEncodingOK = true;
#ifdef _WIN32
        const UINT acp = GetACP();
        const bool ansi = AreFileApisANSI() != FALSE;
        fileEncodingOK = acp == CP_UTF8 && ansi;
#endif
        const bool targetPresent = request.expectedVendor.has_value() && request.expectedRenderer.has_value();
        const bool targetMatches = targetPresent && *request.expectedVendor == vendor && *request.expectedRenderer == renderer;
        const bool hardwareMatched = request.profile == "opengl-hardware" && targetMatches
            && !software && contextOK && fileEncodingOK;
        const bool softwareMatched = request.profile == "opengl-software" && llvmpipe && contextOK && fileEncodingOK;
        result.environment = "{\"requested_api\":\"OpenGL 3.3 core\",\"gl_vendor\":" + verification::quote(vendor)
            + ",\"gl_renderer\":" + verification::quote(renderer) + ",\"gl_version\":" + verification::quote(version)
            + ",\"expected_gl_vendor\":" + (request.expectedVendor ? verification::quote(*request.expectedVendor) : "null")
            + ",\"expected_gl_renderer\":" + (request.expectedRenderer ? verification::quote(*request.expectedRenderer) : "null")
            + ",\"glsl_version\":" + verification::quote(glsl) + ",\"context_thread\":" + verification::quote(thread.str())
            + ",\"context_address\":" + verification::quote(current.str())
            + ",\"context_major\":" + std::to_string(major) + ",\"context_minor\":" + std::to_string(minor)
            + ",\"context_profile_mask\":" + std::to_string(mask)
            + ",\"context_current\":" + (glfwGetCurrentContext() ? "true" : "false")
            + ",\"share_group\":\"unshared; GLFW share=null\",\"qualification\":"
            + verification::quote(hardwareMatched ? "MATCHED_EXPECTATION" : softwareMatched ? "SOFTWARE_LLVMPIPE" : "UNVERIFIED")
            + ",\"qualification_scope\":\"declared/observed target only; not universal physical acceleration or advanced capabilities\"";
#ifdef _WIN32
        result.environment += ",\"process_acp\":" + std::to_string(acp) + ",\"file_apis_ansi\":" + (ansi ? "true" : "false");
#endif
        result.environment += '}';
        checkGL("renderer identity");
        require(contextOK, "observed context is not current OpenGL 3.3+ core", 20);
        // Capabilities, not an OS version guess. The local manifest requests
        // UTF-8 on Windows 1903+; reject unavailable ACP/file mode before the
        // production Reader consumes any narrow UTF-8 shader filename.
        require(fileEncodingOK, "UTF-8 ACP/ANSI file API unavailable before shader Reader", 20);
        if (request.profile == "opengl-hardware")
        {
            require(!software, "software renderer refuses requested hardware profile", 20);
            require(targetPresent, "hardware target UNVERIFIED: expected vendor/renderer is absent", 20);
            require(targetMatches, "hardware target UNVERIFIED: exact vendor/renderer mismatch", 20);
            return; // Actual context/target checks precede every program/upload/draw.
        }
#ifdef _WIN32
        throw verification::Failure(20, "Windows software profile is unavailable in v1");
#else
        require(llvmpipe, "software profile requires explicitly requested llvmpipe, no silent fallback", 20);
#endif
    }

    Pass loadedPass()
    {
        const std::filesystem::path root(ARTIST_VERIFICATION_SOURCE_ROOT);
        std::vector<std::unique_ptr<pipeline::IShader<API>>> shaders;
        shaders.push_back(std::make_unique<pipeline::Shader<API, profile::Shader::Classic>>(
            verification::utf8Bytes((root / "test-datas/shaders/vertex/good/f2-single-values.vert").u8string()),
            artist::graphic::context::ShaderType::VERTEX));
        shaders.push_back(std::make_unique<pipeline::Shader<API, profile::Shader::Classic>>(
            verification::utf8Bytes((root / "test-datas/shaders/fragment/good/minimal.frag").u8string()),
            artist::graphic::context::ShaderType::FRAGMENT));
        Pass pass(std::move(shaders));
        pass.load(); // Production compile/link/reflection and their diagnostics.
        return pass;
    }

    struct ProgramBinding
    {
        explicit ProgramBinding(Pass &pass) { pass.use(); }
        ~ProgramBinding() { glUseProgram(0); }
    };

    struct TriangleTarget
    {
        GLuint framebuffer = 0, color = 0, vao = 0, vbo = 0;
        GLsizei grid, extent;
        explicit TriangleTarget(std::uint64_t batch)
            : grid(batch == 1 ? 1 : batch == 16 ? 4 : 16), extent(grid * cellExtent)
        {
            try
            {
                GLint renderbufferLimit = 0;
                glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &renderbufferLimit);
                require(renderbufferLimit >= extent, "renderbuffer extent unavailable", 20);
                glGenFramebuffers(1, &framebuffer);
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
                glGenRenderbuffers(1, &color);
                glBindRenderbuffer(GL_RENDERBUFFER, color);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, extent, extent);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color);
                glDrawBuffer(GL_COLOR_ATTACHMENT0);
                glReadBuffer(GL_COLOR_ATTACHMENT0);
                require(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FBO incomplete", 20);
                GLint encoding = 0, samples = -1, format = 0;
                glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                    GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING, &encoding);
                glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &samples);
                glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &format);
                require(encoding == GL_LINEAR && samples == 0 && format == GL_RGBA8,
                    "FBO is not single-sample linear RGBA8", 20);
                constexpr std::array<GLfloat, 9> vertices{
                    -.75f, -.75f, 0.f, .75f, -.75f, 0.f, 0.f, .75f, 0.f};
                glGenVertexArrays(1, &vao);
                glBindVertexArray(vao);
                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
                glEnableVertexAttribArray(0);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                for (GLenum state : {GL_BLEND, GL_DITHER, GL_FRAMEBUFFER_SRGB, GL_MULTISAMPLE,
                    GL_CULL_FACE, GL_SCISSOR_TEST, GL_DEPTH_TEST, GL_STENCIL_TEST,
                    GL_RASTERIZER_DISCARD, GL_POLYGON_SMOOTH, GL_COLOR_LOGIC_OP,
                    GL_SAMPLE_COVERAGE, GL_SAMPLE_ALPHA_TO_COVERAGE}) glDisable(state);
                glClearColor(0.f, 0.f, 0.f, 1.f);
                glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
                glPixelStorei(GL_PACK_ALIGNMENT, 1);
                glPixelStorei(GL_PACK_ROW_LENGTH, 0);
                glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
                glPixelStorei(GL_PACK_SKIP_ROWS, 0);
                checkGL("triangle/FBO setup");
            }
            catch (...) { release(); throw; }
        }
        TriangleTarget(const TriangleTarget &) = delete;
        TriangleTarget &operator=(const TriangleTarget &) = delete;
        void release() noexcept
        {
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteBuffers(1, &vbo);
            glDeleteVertexArrays(1, &vao);
            glDeleteRenderbuffers(1, &color);
            glDeleteFramebuffers(1, &framebuffer);
        }
        ~TriangleTarget() { release(); }
        void viewport(std::uint64_t cell) const
        {
            const auto x = static_cast<GLint>(cell % grid) * cellExtent;
            const auto y = static_cast<GLint>(cell / grid) * cellExtent;
            glViewport(x, y, cellExtent, cellExtent);
        }
        void beginPhase() const
        {
            // glClear ignores viewport. Clear exactly once for the whole phase,
            // never inside the cell loop (which would erase previous cells).
            glClear(GL_COLOR_BUFFER_BIT);
        }
        void verify(float value, std::ostringstream *evidence = nullptr) const
        {
            std::vector<GLubyte> image(static_cast<std::size_t>(extent) * extent * 4);
            glReadPixels(0, 0, extent, extent, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
            checkGL("image readback outside timing");
            constexpr std::array points{std::array{8, 8}, std::array{24, 24},
                std::array{18, 16}, std::array{2, 2}};
            const bool shifted = value == .5f;
            const std::array expected{shifted ? background : white,
                shifted ? white : background, white, background};
            for (GLsizei cell = 0; cell < grid * grid; ++cell)
            {
                if (evidence)
                {
                    if (cell) *evidence << ',';
                    *evidence << "{\"cell\":" << cell << ",\"pixels\":[";
                }
                for (std::size_t point = 0; point < points.size(); ++point)
                {
                    const auto x = (cell % grid) * cellExtent + points[point][0];
                    const auto y = (cell / grid) * cellExtent + points[point][1];
                    const auto offset = static_cast<std::size_t>((y * extent + x) * 4);
                    const Pixel actual{image[offset], image[offset + 1], image[offset + 2], image[offset + 3]};
                    if (evidence)
                    {
                        if (point) *evidence << ',';
                        *evidence << '[' << unsigned(actual[0]) << ',' << unsigned(actual[1])
                            << ',' << unsigned(actual[2]) << ',' << unsigned(actual[3]) << ']';
                    }
                    if (actual != expected[point])
                    {
                        std::cerr << "pixel oracle: cell=" << cell << " x=" << x << " y=" << y
                            << " scalar=" << value << " rgba=" << unsigned(actual[0]) << ','
                            << unsigned(actual[1]) << ',' << unsigned(actual[2]) << ',' << unsigned(actual[3]) << '\n';
                        throw verification::Failure(10, "fixed independent pixel oracle failed");
                    }
                }
                if (evidence) *evidence << "]}";
            }
        }
    };

    // Pass-through driver observations in a distinct, serialized control only.
    // No spy survives into a measured window. glDrawArrays is a GL 1.1 entry on
    // Windows, not a GLEW pointer; its control uses an explicit call-site count
    // plus the independent pixel oracle, rather than inventing a GLEW hook.
    class NativeUploadControl
    {
        PFNGLUNIFORM1FPROC originalUpload;
        PFNGLGETUNIFORMLOCATIONPROC originalLookup;
        static inline NativeUploadControl *active = nullptr;
        static void GLAPIENTRY upload(GLint location, GLfloat value)
        {
            ++active->uploads;
            active->originalUpload(location, value);
        }
        static GLint GLAPIENTRY lookup(GLuint program, const GLchar *name)
        {
            ++active->lookups;
            return active->originalLookup(program, name);
        }
    public:
        std::uint64_t uploads = 0, lookups = 0;
        NativeUploadControl() : originalUpload(__glewUniform1f), originalLookup(__glewGetUniformLocation)
        {
            require(active == nullptr && originalUpload && originalLookup, "native observation entrypoints unavailable", 20);
            active = this;
            __glewUniform1f = &upload;
            __glewGetUniformLocation = &lookup;
        }
        NativeUploadControl(const NativeUploadControl &) = delete;
        NativeUploadControl &operator=(const NativeUploadControl &) = delete;
        ~NativeUploadControl()
        {
            __glewUniform1f = originalUpload;
            __glewGetUniformLocation = originalLookup;
            active = nullptr;
        }
    };

    struct NoCellCheck { void operator()(float) const noexcept {} };

    template<class Setter, class Draw, class CellCheck = NoCellCheck>
    void phase(const verification::Request &request, TriangleTarget &target, Setter &set, Draw draw,
        float value, CellCheck cellCheck = {})
    {
        target.beginPhase();
        for (std::uint64_t cell = 0; cell < request.batch; ++cell)
        {
            target.viewport(cell);
            set(value); // Production context/program/generation guards stay active.
            draw();
            cellCheck(value); // Empty in measured kernel; real checks in distinct control.
        }
    }

    template<class Setter>
    void exercise(const verification::Request &request, verification::Result &result,
        Pass &pass, TriangleTarget &target, Setter set, std::int64_t programPreparationNs,
        std::int64_t accessPreparationNs)
    {
        const GLuint program = pass.getContext().getPassID();
        const GLint location = glGetUniformLocation(program, "f2Scalar");
        require(location >= 0, "f2Scalar native location unavailable", 20);
        const auto &mirror = *pass.getUniforms().at("f2Scalar");
        auto verifyScalar = [&](float value)
        {
            GLfloat actual = -1.f;
            glGetUniformfv(program, location, &actual);
            require(actual == value && mirror.get<GLfloat>() == value, "CPU/native scalar oracle failed");
            checkGL("scalar readback outside timing");
        };
        std::ostringstream pixels;
        pixels.imbue(std::locale::classic());
        std::uint64_t controlDraws = 0;
        {
            NativeUploadControl control;
            const auto calibrated = glGetUniformLocation(program, "f2Scalar");
            glUniform1f(calibrated, 0.f);
            require(calibrated == location && control.lookups == 1 && control.uploads == 1,
                "native control calibration failed", 30);
            control.uploads = control.lookups = 0;
            pixels << '[';
            for (std::size_t index = 0; index < phaseValues.size(); ++index)
            {
                if (index) pixels << ',';
                pixels << "{\"value\":" << phaseValues[index] << ",\"cells\":[";
                auto countedDraw = [&]
                {
                    ++controlDraws;
                    glDrawArrays(GL_TRIANGLES, 0, 3);
                };
                // Same phase kernel as the measured path, with checks enabled
                // for every cell outside all timing.
                phase(request, target, set, countedDraw, phaseValues[index], verifyScalar);
                target.verify(phaseValues[index], &pixels);
                pixels << "]}";
            }
            pixels << ']';
            require(control.lookups == 0 && control.uploads == 3 * request.batch
                && controlDraws == 3 * request.batch, "work control observed missing or extra work");
        } // Native pointers restored before warmup/timing.

        const auto draw = [] { glDrawArrays(GL_TRIANGLES, 0, 3); };
        for (float value : phaseValues) phase(request, target, set, draw, value);
        target.verify(0.f);
        verifyScalar(0.f);

        constexpr double declaredTickNs = 1e9 * double(verification::Clock::period::num)
            / double(verification::Clock::period::den);
        std::int64_t observedTick = (std::numeric_limits<std::int64_t>::max)();
        auto previous = verification::Clock::now();
        for (int i = 0; i < 1024; ++i)
        {
            const auto current = verification::Clock::now();
            const auto delta = verification::elapsed(previous, current);
            if (delta > 0) observedTick = (std::min)(observedTick, delta);
            previous = current;
        }
        const bool clockObserved = observedTick != (std::numeric_limits<std::int64_t>::max)();
        bool measurementValid = request.repetitions >= 3 && clockObserved;
        const double minimumElapsed = 100.0 * (std::max)(declaredTickNs, clockObserved ? double(observedTick) : 1.0);
        std::vector<float> values(request.iterations); // Prepared outside every interval.
        std::vector<std::int64_t> aa, updatesOnly;
        aa.reserve(request.repetitions);
        updatesOnly.reserve(request.repetitions);
        for (std::uint64_t repetition = 0; repetition < request.repetitions; ++repetition)
        {
            for (std::uint64_t iteration = 0; iteration < request.iterations; ++iteration)
                values[iteration] = phaseValues[(iteration + repetition) % phaseValues.size()];
            if (!request.measurementRequested)
            {
                for (float value : values)
                {
                    phase(request, target, set, draw, value);
                    result.updates += request.batch;
                    result.draws += request.batch;
                    target.verify(value);
                    verifyScalar(value);
                }
                continue;
            }
            // Main useful submission. No logs/readbacks/assertions/instrumented
            // counters in this window; production guards inside set() remain.
            const auto before = verification::Clock::now();
            for (float value : values) phase(request, target, set, draw, value);
            const auto after = verification::Clock::now();
            const auto nanoseconds = verification::elapsed(before, after);
            result.updates += request.population();
            result.draws += request.population();
            if (nanoseconds > 0)
                result.samples.push_back({nanoseconds, repetition, request.population()});
            measurementValid = measurementValid && nanoseconds >= minimumElapsed;
            target.verify(values.back()); // Also drains prior draws, outside timer.
            verifyScalar(values.back());

            // Explicit updates-only microdiagnostic, not a useful-render sample.
            const auto updatesBefore = verification::Clock::now();
            for (float value : values)
                for (std::uint64_t cell = 0; cell < request.batch; ++cell) set(value);
            const auto updatesAfter = verification::Clock::now();
            updatesOnly.push_back(verification::elapsed(updatesBefore, updatesAfter));
            measurementValid = measurementValid && updatesOnly.back() >= minimumElapsed;
            verifyScalar(values.back());

            // Same-route A/A, identical kernel and prepared inputs. No claimed
            // speedup. Its calls are a separate diagnostic population.
            const auto aaBefore = verification::Clock::now();
            for (float value : values) phase(request, target, set, draw, value);
            const auto aaAfter = verification::Clock::now();
            aa.push_back(verification::elapsed(aaBefore, aaAfter));
            target.verify(values.back());
            verifyScalar(values.back());
            measurementValid = measurementValid && aa.back() >= minimumElapsed;
            if (nanoseconds > 0)
            {
                const double drift = std::abs(double(aa.back()) - double(nanoseconds)) / double(nanoseconds);
                measurementValid = measurementValid && drift <= .25;
            }
        }
        std::ostringstream observations;
        observations.imbue(std::locale::classic());
        observations << std::setprecision(17)
            << "{\"control_sequence\":[0,0.5,0],\"pixel_points\":[[8,8],[24,24],[18,16],[2,2]],"
            "\"pixel_evidence\":" << pixels.str() << ",\"control_native_uploads\":" << 3 * request.batch
            << ",\"control_draw_calls\":" << controlDraws << ",\"control_native_lookups\":0,"
            "\"control_calibration\":{\"native_lookup\":1,\"native_upload\":1},"
            "\"draw_observation\":\"call-site count forwarding real glDrawArrays plus pixel oracle\","
            "\"main_counts\":\"completed kernel calls; not driver instrumentation in timed window\","
            "\"counted_population\":\"main only; control, warmup, updates-only and A/A excluded\","
            "\"warmup_updates\":" << 3 * request.batch << ",\"warmup_draws\":" << 3 * request.batch
            << ",\"program_preparation_wall_ns\":" << programPreparationNs
            << ",\"access_preparation_wall_ns\":";
        if (request.route == "prepared-float") observations << accessPreparationNs;
        else observations << "null";
        observations << ",\"clock\":\"steady_clock CPU wall; not CPU thread or GPU time\","
            "\"declared_tick_ns\":" << declaredTickNs << ",\"observed_min_positive_tick_ns\":";
        if (clockObserved) observations << observedTick; else observations << "null";
        observations << ",\"minimum_elapsed_ns\":" << minimumElapsed
            << ",\"minimum_repetitions\":3,\"aa_relative_drift_limit\":0.25,"
            "\"noise_policy_status\":\"candidate; not platform-qualified\","
            "\"order\":\"control(0,.5,0), warmup(0,.5,0); per repetition main, updates-only, same-route A/A\","
            "\"phase_policy\":\"[0,.5,0][(iteration+repetition)%3]; prepared outside interval\","
            "\"updates_only_wall_ns\":[";
        for (std::size_t i = 0; i < updatesOnly.size(); ++i)
        {
            if (i) observations << ',';
            observations << updatesOnly[i];
        }
        observations << "],\"same_route_aa_wall_ns\":[";
        for (std::size_t i = 0; i < aa.size(); ++i)
        {
            if (i) observations << ',';
            observations << aa[i];
        }
        observations << "],\"cell_oracle_limit\":\"identical-cell bijective permutations are invisible\","
            "\"timed_image_oracle_limit\":\"only final phase read back; separate 0/.5/0 control does not prove every intermediate measured image\","
            "\"guards\":\"production context/program/generation checks enabled\","
            "\"waits\":\"blocking pixel readbacks after intervals; no glFinish\"}";
        result.observations = observations.str();
        result.verdict = "PASS";
        if (request.measurementRequested)
            result.validity = measurementValid && result.samples.size() == request.repetitions ? "VALID" : "INVALID";
    }

    void workload(const verification::Request &request, verification::Result &result)
    {
#ifdef _WIN32
        require(request.profile != "opengl-software", "Windows software profile is unavailable in v1", 20);
#endif
        NativeContext context; // First owner, last destroyed.
        qualifyRenderer(request, result, context.window); // Exact target/owned context before uploads/draws.
        {
            const auto preparationBefore = verification::Clock::now();
            auto pass = loadedPass();
            const auto preparationAfter = verification::Clock::now();
            ProgramBinding binding(pass); // Unbinds before pass destruction.
            TriangleTarget target(request.batch); // Retires before binding/pass/context.
            pass.withUniform("f2Matrix", glm::mat3(1.f));
            pass.withUniform("f2Vector", glm::vec3(0.f));
            pass.withUniform("testUniform", GLint(1));
            const auto arrayLocation = glGetUniformLocation(pass.getContext().getPassID(), "f2Array[0]");
            require(arrayLocation >= 0, "array fixture location missing", 20);
            constexpr std::array<GLfloat, 6> zeroArray{};
            glUniform3fv(arrayLocation, 2, zeroArray.data());
            checkGL("fixed shader parameter initialization");
            const std::string scalarName = "f2Scalar"; // No name/value construction in interval.
            const auto programNs = verification::elapsed(preparationBefore, preparationAfter);
            if (request.route == "pass-name")
            {
                auto set = [&](float value) { pass.withUniform(scalarName, value); };
                exercise(request, result, pass, target, set, programNs, 0);
            }
            else
            {
                const auto accessBefore = verification::Clock::now();
                const auto access = artist::graphic::opengl::pipeline::prepareFloatUniform(pass, scalarName);
                const auto accessAfter = verification::Clock::now();
                auto set = [&](float value) { access.set(value); };
                exercise(request, result, pass, target, set, programNs,
                    verification::elapsed(accessBefore, accessAfter));
            }
        }
        require(glfwGetCurrentContext() == context.window, "context changed before resource retirement", 30);
        checkGL("resource retirement in current context");
    }
}

#ifdef _WIN32
int wmain(int argc, wchar_t **argv)
#else
int main(int argc, char **argv)
#endif
{
    return artist::verification::mainWithResult(argc, argv, true, workload);
}
