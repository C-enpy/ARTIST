// file: Reader.hpp

#pragma once

#include <string>
#include <graphic/Api.hpp>
#include <graphic/opengl/context/ShaderContext.hpp>
#include <fstream>
#include <format>
#include <common/exception/TraceableException.hpp>
#include <graphic/opengl/profile/Shader.hpp>

namespace artist::graphic::opengl::pipeline::component::shader
{
    template <auto PROFILE>
    class OpenGLShaderReader
    {
    };

    template <>
    class OpenGLShaderReader<graphic::opengl::profile::Shader::Classic>
    {
    public:
        static void on(typename graphic::api::OpenGL::ShaderContext &context)
        {
            context.setShaderCode("");
            std::ifstream shaderFile;

            // F8 (mission C): only badbit stays in the exception mask, as
            // before: an impossible opening and later stream failures do not
            // throw by themselves; they are reported through the stream state
            // and checked explicitly below.
            shaderFile.exceptions(std::ifstream::badbit);

            try
            {
                shaderFile.open(context.getShaderPath());

                // F8: extract through an explicit read()/gcount() loop instead
                // of the streambuf hand-off (shaderStream << rdbuf): every
                // block's count is observed, so a failed opening or a short
                // extraction can no longer turn into a silent empty source
                // handed to the loader. The code string is published only
                // after the file is closed and every state is verified; a
                // process-level bad_alloc while growing the target string is
                // NEVER remapped as a file error (only std::ifstream::failure
                // is translated, below).
                std::string code;
                char buffer[4096];
                while (shaderFile.read(buffer, sizeof buffer))
                {
                    code.append(buffer, static_cast<std::size_t>(shaderFile.gcount()));
                }
                // The last extraction was short (or empty at the exact end of
                // the file), ending with failbit+eofbit: that is the ruled
                // normal termination, and its partial content still counts.
                code.append(buffer, static_cast<std::size_t>(shaderFile.gcount()));

                // failbit without eofbit - e.g. the reported opening failure:
                // open() sets failbit and the read() above keeps it without
                // reaching an end-of-file - is a read error of the source,
                // never a compilable empty source.
                if (shaderFile.fail() && !shaderFile.eof())
                {
                    throw common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\nReading stopped without reaching end-of-file.");
                }

                // Documented phase boundary: the read phase has been checked
                // and its bytes accumulated above, so the eof/fail residue is
                // cleared to make the close() result observable on its own;
                // no information is lost here.
                shaderFile.clear();
                shaderFile.close();
                if (shaderFile.fail())
                {
                    throw common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\nClosing the file failed.");
                }

                // Publication happens only after a verified close.
                context.setShaderCode(code);
            }
            catch (std::ifstream::failure &e)
            {
                // A real thrown extraction failure (badbit) keeps its mapped
                // business nature; other standard exceptions (e.g. bad_alloc
                // from this process) propagate unchanged.
                throw common::exception::TraceableException<std::runtime_error>(std::format("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n{}", e.what()));
            }
        }
    };
}
