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
    // F8-SEAM (ruling G-F8): the extraction is factored into a header-only
    // internal detail seam so a test can drive the same loop with a scripted
    // streambuf and prove the real badbit failure branch. It stays at most
    // one call per loaded file, never per block/draw/frame; no performance
    // gain or de-virtualization is claimed.
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

        /// Signature of the extraction seam: a plain function of the live
        /// stream. The callback is synchronous, called at most once per
        /// loaded file, never retained nor nulled, and never closes,
        /// replaces or keeps the file.
        using SourceExtractor = std::string(std::istream &);

        /// The loading transaction: clear the context, open a local owning
        /// file, extract into a local string, then clear/close/verify and
        /// publish. Only std::ifstream::failure is translated into the
        /// public read error; every other exception (e.g. bad_alloc from
        /// this process) keeps its nature and the context stays empty.
        inline void readShaderFile(graphic::api::OpenGL::ShaderContext &context, SourceExtractor &extract)
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

                // F8-SEAM: extraction through the ruled read()/gcount() loop
                // (the streambuf hand-off shaderStream << rdbuf is still
                // avoided: every block's count is observed, so a failed
                // opening or a short extraction can no longer turn into a
                // silent empty source handed to the loader). The code string
                // is published only after the file is closed and every state
                // is verified; a process-level bad_alloc while growing the
                // target string is NEVER remapped as a file error (only
                // std::ifstream::failure is translated, below).
                std::string code = extract(shaderFile);

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
            detail::readShaderFile(context, detail::extractShaderSource);
        }
    };
}
