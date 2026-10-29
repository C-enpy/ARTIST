// file: Reader.hpp

#pragma once

#include <string>
#include <graphic/Api.hpp>
#include <graphic/opengl/context/ShaderContext.hpp>
#include <fstream>
#include <istream>
#include <format>
#include <common/exception/TraceableException.hpp>
#include <graphic/opengl/profile/Shader.hpp>

namespace artist::graphic::opengl::pipeline::component::shader
{
    namespace detail
    {
        /// Extracts the whole stream through the ruled read()/gcount() loop
        /// and rejects a stop without end-of-file. Precondition: a live
        /// stream whose exception mask keeps badbit active. The extractor
        /// neither touches the context, nor the mask, nor the stream
        /// lifetime: it never closes, replaces, keeps or translates the
        /// error (only the ruled fail-without-EOF TraceableException is
        /// raised, as before).
        inline std::string extractShaderSource(std::istream &source)
        {
            std::string code;
            char buffer[4096];
            while (source.read(buffer, sizeof buffer))
            {
                code.append(buffer, static_cast<std::size_t>(source.gcount()));
            }
            // The last extraction was short (or empty at the exact end of
            // the file), ending with failbit+eofbit: that is the ruled
            // normal termination, and its partial content still counts.
            code.append(buffer, static_cast<std::size_t>(source.gcount()));

            // failbit without eofbit - e.g. the reported opening failure:
            // open() sets failbit and the read() above keeps it without
            // reaching an end-of-file - is a read error of the source,
            // never a compilable empty source.
            if (source.fail() && !source.eof())
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\nReading stopped without reaching end-of-file.");
            }
            return code;
        }

        /// The loading transaction: clear the context, open a local owning
        /// file, extract into a local string, then clear/close/verify and
        /// publish. Only std::ifstream::failure is translated into the
        /// public read error; every other exception (e.g. bad_alloc from
        /// this process) keeps its nature and the context stays empty.
        // The source file must remain stable for the duration of this call.
        inline void readShaderFile(graphic::api::OpenGL::ShaderContext &context)
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

                // Accumulate locally; no partial source is published on failure.
                std::string code = extractShaderSource(shaderFile);

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
    }

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
            detail::readShaderFile(context);
        }
    };
}
