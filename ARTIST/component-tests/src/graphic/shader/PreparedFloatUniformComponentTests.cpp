#include <OpenGLComponentTests.hpp>
#include <graphic/opengl/pipeline/PreparedFloatUniform.hpp>
#include <graphic/opengl/pipeline/component/pass/UniformReader.hpp>
#include <graphic/opengl/pipeline/component/pass/Freer.hpp>
#include <memory>
#include <stdexcept>

namespace
{
    using Context = artist::graphic::api::OpenGL::PassContext;
    using Reader = Context::UniformReader<artist::graphic::opengl::profile::Pass::Classic>;
    using Error = artist::common::exception::TraceableException<std::runtime_error>;
    using artist::graphic::opengl::pipeline::prepareFloatUniform;

    struct Program
    {
        GLuint id = glCreateProgram();
        Program()
        {
            const char *sources[] = {
                "#version 330 core\nuniform float scalar; uniform float one[1]; uniform vec2 vector;"
                "uniform float inactive; void main(){gl_Position=vec4(scalar+one[0]+vector.x,0,0,1);}",
                "#version 330 core\nout vec4 color; void main(){color=vec4(1);}"};
            try
            {
                for (int stage = 0; stage != 2; ++stage)
                {
                    const GLuint shader = glCreateShader(stage == 0 ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
                    glShaderSource(shader, 1, &sources[stage], nullptr);
                    glCompileShader(shader);
                    GLint compiled = 0;
                    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
                    if (compiled != GL_TRUE)
                    {
                        glDeleteShader(shader);
                        throw std::runtime_error("prepared-float fixture shader did not compile");
                    }
                    glAttachShader(id, shader);
                    glDeleteShader(shader); // GL retains it until program retirement
                }
                glLinkProgram(id);
                GLint linked = 0;
                glGetProgramiv(id, GL_LINK_STATUS, &linked);
                if (linked != GL_TRUE) throw std::runtime_error("prepared-float fixture link failed");
            }
            catch (...) { glDeleteProgram(id); throw; }
        }
        Program(const Program &) = delete;
        Program &operator=(const Program &) = delete;
        ~Program() { glUseProgram(0); glDeleteProgram(id); }
    };
}

class PreparedFloatUniformComponentTest : public OpenGLComponentTest
{
protected:
    void SetUp() override
    {
        OpenGLComponentTest::SetUp();
        if (IsSkipped() || HasFatalFailure()) return;
        while (glGetError() != GL_NO_ERROR) {} // only GLEW initialization
    }
};

TEST_F(PreparedFloatUniformComponentTest, StrictReflectionAndTypedReadback)
{
    Program program;
    Context context;
    context.setPassID(program.id);
    Reader::on(context);
    ASSERT_EQ(glGetError(), GL_NO_ERROR);
    EXPECT_THROW(prepareFloatUniform(context, "inactive"), Error);
    EXPECT_THROW(prepareFloatUniform(context, "absent"), Error);
    EXPECT_THROW(prepareFloatUniform(context, "vector"), Error);
    // Actual GL reflection exposes the size-one array under its indexed name.
    ASSERT_NE(context.getUniform("one[0]"), nullptr);
    EXPECT_THROW(prepareFloatUniform(context, "one[0]"), Error);
    auto access = prepareFloatUniform(context, "scalar");
    glUseProgram(program.id);
    access.set(0);
    access.set(.5f);
    access.set(-.25f);
    const auto *uniform = context.getUniform("scalar");
    ASSERT_NE(uniform, nullptr);
    GLfloat actual = 0;
    glGetUniformfv(program.id, static_cast<GLint>(uniform->getContext().getUniformID()), &actual);
    EXPECT_EQ(actual, -.25f);
    EXPECT_EQ(uniform->get<GLfloat>(), -.25f);
    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(PreparedFloatUniformComponentTest, WrongProgramAndAbsentContextPreserveCpuAndDriver)
{
    Program target, decoy;
    Context context;
    context.setPassID(target.id);
    Reader::on(context);
    auto access = prepareFloatUniform(context, "scalar");
    glUseProgram(target.id);
    access.set(.25f);
    glUseProgram(decoy.id);
    EXPECT_THROW(access.set(.5f), Error);
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), decoy.id);
    glfwMakeContextCurrent(nullptr);
    EXPECT_THROW(access.set(.5f), Error);
    glfwMakeContextCurrent(window);
    const auto *uniform = context.getUniform("scalar");
    EXPECT_EQ(uniform->get<GLfloat>(), .25f);
    GLfloat actual = 0;
    glGetUniformfv(target.id, static_cast<GLint>(uniform->getContext().getUniformID()), &actual);
    EXPECT_EQ(actual, .25f);
    EXPECT_EQ(glGetError(), GL_NO_ERROR);
    glUseProgram(target.id);
    access.set(.75f);
    glGetUniformfv(target.id, static_cast<GLint>(uniform->getContext().getUniformID()), &actual);
    EXPECT_EQ(actual, .75f);
    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}
