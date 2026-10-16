#ifdef __mock_gl__

#include <gtest/gtest.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <new>
#include <optional>
#include <sstream>
#include <string>
#include <system_error>
#include <thread>
#include <opengl/glFunctionMock.hpp>
#include <graphic/MockApi.hpp>
#include <graphic/opengl/profile/Shader.hpp>
#include <graphic/opengl/context/ShaderContext.hpp>
#include <graphic/opengl/pipeline/component/shader/Reader.hpp>
#include <TestUtils.hpp>

namespace context = artist::graphic::opengl::context;
namespace shader = artist::graphic::opengl::pipeline::component::shader;
namespace mock = artist::mock;
namespace api = artist::graphic::api;
using artist::common::exception::TraceableException;
using artist::graphic::opengl::profile::Shader::Classic;
using artist::test::utils::expectSpecificError;

namespace
{
    // F8-SEAM cycle 1 (H-TEMP-LOCK) + isolation (finding f8a18a8): the old
    // fixture removed the whole shared %TEMP%/artist-reader-tests tree
    // wholesale, so suites could not coexist, and absentTempShaderPath
    // ignored the std::filesystem::remove error - a surviving residue
    // silently voided the "guaranteed absent" guarantee and
    // ReadShaderTest_AbsentFileRejected failed as "expected an exception"
    // under a transient external lock (docs/development/known-incidents.md).
    // This suite now owns a DISTINCT sibling parent,
    // %TEMP%/artist-reader-tests-owned, never the legacy shared parent:
    // older worktrees still running the pre-F8-SEAM fixture tear
    // %TEMP%/artist-reader-tests down wholesale in their TearDown and would
    // destroy this suite's active owned roots mid-run. Each fixture instance
    // owns a unique child directory; the exclusivity proof is
    // std::filesystem::create_directory's atomic success, not the
    // (timestamp/random) name, which alone proves nothing.
    const std::filesystem::path &readerFixturesParent()
    {
        static const std::filesystem::path parent =
            std::filesystem::temp_directory_path() / "artist-reader-tests-owned";
        return parent;
    }

    constexpr std::size_t maxOwnedRootAttempts = 16;

    std::string uniqueRootSuffix()
    {
        static std::atomic<unsigned long long> counter{0};
        const auto tick = std::chrono::steady_clock::now().time_since_epoch().count();
        std::ostringstream name;
        name << "fixture-" << std::hash<std::thread::id>{}(std::this_thread::get_id())
             << '-' << tick << '-' << counter.fetch_add(1);
        return name.str();
    }

    /// True when <ancestor> is a component-wise path-prefix ancestor of
    /// <path> (both canonically normalized); a path is not its own ancestor.
    /// Used by the isolation test only, never for cleanup.
    bool isAncestorPath(const std::filesystem::path &ancestor, const std::filesystem::path &path)
    {
        auto ancestorComponent = ancestor.begin();
        auto pathComponent = path.begin();
        for (; ancestorComponent != ancestor.end(); ++ancestorComponent, ++pathComponent)
        {
            if (pathComponent == path.end() || *ancestorComponent != *pathComponent)
            {
                return false;
            }
        }
        return ancestor != path;
    }

    /// Acquires a directory this instance exclusively owns: create_directory
    /// returning true is the atomic exclusivity proof (we created it, it did
    /// not pre-exist); a mere name collision (false without error) is treated
    /// by retrying with a fresh suffix. nullopt = setup failure the caller
    /// must diagnose - never a skip.
    std::optional<std::filesystem::path> tryAcquireOwnedRoot(const std::filesystem::path &parent)
    {
        std::error_code ec;
        std::filesystem::create_directories(parent, ec);
        if (ec)
        {
            return std::nullopt;
        }
        for (std::size_t attempt = 0; attempt < maxOwnedRootAttempts; ++attempt)
        {
            const auto candidate = parent / uniqueRootSuffix();
            ec.clear();
            if (std::filesystem::create_directory(candidate, ec))
            {
                return candidate;
            }
            if (ec)
            {
                return std::nullopt;
            }
        }
        return std::nullopt;
    }

    /// Removes exactly the owned root. False (+ ec) when something survived
    /// (e.g. an external lock): bounded cleanup, reported, never spread to a
    /// neighbor directory. Idempotent on an already-removed root.
    bool tryCleanupOwnedRoot(const std::filesystem::path &root, std::error_code &cleanupError)
    {
        cleanupError.clear();
        if (root.empty())
        {
            return true;
        }
        std::filesystem::remove_all(root, cleanupError);
        std::error_code stillThere;
        return !std::filesystem::exists(root, stillThere) && !stillThere;
    }

    /// Guarantees an absent path inside an owned root: the residue is removed
    /// and its survival (locked file, non-empty directory, stat error) is
    /// reported as nullopt, never waived silently (H-TEMP-LOCK). Pure
    /// filesystem logic with no gtest side effect, so the guarantee itself is
    /// directly testable; the fixture glue below turns nullopt into a precise
    /// setup failure.
    std::optional<std::filesystem::path> tryGuaranteeAbsentPath(const std::filesystem::path &root, const std::string &fileName)
    {
        if (root.empty() || fileName.empty())
        {
            return std::nullopt;
        }
        const auto path = root / fileName;
        std::error_code removeError;
        std::filesystem::remove(path, removeError);
        std::error_code stillThere;
        if (std::filesystem::exists(path, stillThere) || stillThere)
        {
            return std::nullopt;
        }
        return path;
    }

    // F8-SEAM cycle 2: seam probes. The readShaderFile signature takes a
    // SourceExtractor function reference (not a lambda), so the failing
    // streambuf is handed over through a real test function below. The state
    // pointer is per-test and the CPU suite is single-threaded; each test
    // installs it in its body and clears it afterwards.

    /// Scripted streambuf: the first xsgetn serves exactly the requested
    /// bytes (the real extractor asks for its ruled 4096-byte block); the
    /// second request is counted, then the scripted failure escapes - a real
    /// ios_base::failure sentinel or a non-ios std::bad_alloc. A short
    /// xsgetn alone is deliberately NOT used: it would set EOF/fail, the
    /// ruled normal termination, not the injected error.
    class ScriptedStreambuf : public std::streambuf
    {
    public:
        explicit ScriptedStreambuf(std::function<void()> onSecondRequest)
            : onSecondRequest_(std::move(onSecondRequest)),
              block_(4096, 'x')
        {
        }

        int requests() const { return requests_; }

        std::streamsize firstRequestBytes() const { return firstRequestBytes_; }

    protected:
        std::streamsize xsgetn(char *destination, std::streamsize requested) override
        {
            ++requests_;
            if (requests_ == 1)
            {
                firstRequestBytes_ = requested;
                const auto served = std::min<std::streamsize>(requested, static_cast<std::streamsize>(block_.size()));
                std::memcpy(destination, block_.data(), static_cast<std::size_t>(served));
                return served;
            }
            onSecondRequest_();
            return 0;
        }

    private:
        std::function<void()> onSecondRequest_;
        std::string block_;
        int requests_ = 0;
        std::streamsize firstRequestBytes_ = 0;
    };

    ScriptedStreambuf *scriptedStreambuf = nullptr;

    /// Test adapter: the transaction opens the REAL fixture file and hands
    /// the live stream to this function, which calls the SAME extractor the
    /// production path uses, on a local istream backed by the failing
    /// streambuf under the same badbit mask. Whatever escapes the extractor,
    /// the stream's badbit is verified set before the exception is rethrown
    /// unchanged - the error is never raised by the callback itself.
    std::string scriptedExtraction(std::istream &realFile)
    {
        EXPECT_TRUE(realFile.good())
            << "the transaction must hand the adapter a real, usable file stream";
        EXPECT_NE(scriptedStreambuf, nullptr)
            << "test wiring: the scripted streambuf must be installed";
        if (scriptedStreambuf == nullptr)
        {
            throw std::runtime_error("test wiring: missing scripted streambuf");
        }
        ScriptedStreambuf &failing = *scriptedStreambuf;
        std::istream source(&failing);
        source.exceptions(std::ifstream::badbit);
        try
        {
            return shader::detail::extractShaderSource(source);
        }
        catch (...)
        {
            // The injected failure crossed the real read loop as a real
            // stream state: badbit is actually set on the stream before the
            // exception is rethrown (MSVC 14.44 iosfwd _CATCH_IO_END: "set
            // badbit and rethrow").
            EXPECT_TRUE(source.bad())
                << "badbit must be actually set on the stream after the injected failure";
            throw;
        }
    }

    /// Test-side context whose virtual setter observes the transaction at the
    /// exact moment of publication: the reader opens its file without
    /// FILE_SHARE_DELETE, so a still-open handle denies deletion on Windows;
    /// a successful remove AT PUBLICATION proves the borrowed filebuf was
    /// closed before the code was published. The observation is only valid
    /// while the transaction call is live (no pending observation after).
    /// The setter also serves the transaction's initial clearing call
    /// (setShaderCode("")), so the probe fires only on the publication of
    /// the expected bytes, never on the clearing step.
    class PublicationProbeContext : public context::OpenGLShaderContext
    {
    public:
        explicit PublicationProbeContext(std::string publishedMarker)
            : publishedMarker_(std::move(publishedMarker))
        {
        }

        void setShaderCode(const std::string &shaderCode) override
        {
            if (shaderCode == publishedMarker_)
            {
                std::error_code removeError;
                removedAtPublication_ = std::filesystem::remove(getShaderPath(), removeError);
                removalDiagnosis_ = removeError.message();
            }
            context::OpenGLShaderContext::setShaderCode(shaderCode);
        }

        bool removedAtPublication_ = false;
        std::string removalDiagnosis_;

    private:
        std::string publishedMarker_;
    };
}

class ReaderTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        const auto ownedRoot = tryAcquireOwnedRoot(readerFixturesParent());
        if (!ownedRoot.has_value())
        {
            FAIL() << "setup: could not acquire an exclusively-owned fixture root under "
                   << readerFixturesParent().string()
                   << " (create_directory failed or kept colliding); no Reader call is safe without it.";
            return;
        }
        ownedRoot_ = *ownedRoot;
    }

    void TearDown() override
    {
        mock::opengl::glFunctionMock::reset();
        std::error_code cleanupError;
        if (!tryCleanupOwnedRoot(ownedRoot_, cleanupError))
        {
            ADD_FAILURE() << "teardown: owned root " << ownedRoot_.string()
                          << " survived its bounded cleanup (" << cleanupError.message()
                          << "); the neighbor directories are untouched.";
        }
    }

    /// Writes a temporary shader source file with full control over the bytes.
    /// Creation (inside the owned root), open, write and close are verified;
    /// nullopt = setup failure the caller must honor (fatal assert before any
    /// Reader call), never a skip.
    std::optional<std::filesystem::path> makeTempShaderSource(const std::string &fileName, const std::string &content)
    {
        if (ownedRoot_.empty())
        {
            ADD_FAILURE() << "setup: no owned fixture root; cannot write " << fileName;
            return std::nullopt;
        }
        const auto path = ownedRoot_ / fileName;
        std::error_code ec;
        if (std::filesystem::exists(path, ec) || ec)
        {
            ADD_FAILURE() << "setup: " << path.string()
                          << " already exists inside the freshly acquired owned root";
            return std::nullopt;
        }
        std::ofstream stream(path, std::ios::binary | std::ios::trunc);
        if (!stream.is_open())
        {
            ADD_FAILURE() << "setup: open failed for " << path.string();
            return std::nullopt;
        }
        stream.write(content.data(), static_cast<std::streamsize>(content.size()));
        const bool writeAccepted = stream.good();
        stream.close();
        if (!writeAccepted || stream.fail())
        {
            ADD_FAILURE() << "setup: write/close verification failed for " << path.string()
                          << " (" << content.size() << " bytes expected)";
            return std::nullopt;
        }
        return path;
    }

    /// Guarantees a path that does not exist (residue removal before the
    /// test); a failed removal or a surviving residue is a precise setup
    /// failure - the caller must stop on nullopt before calling the Reader
    /// (H-TEMP-LOCK: the old helper ignored the removal error and let a
    /// readable residue turn the rejection test into a silent pass).
    std::optional<std::filesystem::path> absentTempShaderPath(const std::string &fileName)
    {
        auto guaranteed = tryGuaranteeAbsentPath(ownedRoot_, fileName);
        if (!guaranteed.has_value())
        {
            ADD_FAILURE() << "setup: the requested-absent path " << (ownedRoot_ / fileName).string()
                          << " could not be guaranteed absent (remove failed or residue survived);"
                          << " the test must not call the Reader on an unverified filesystem state.";
        }
        return guaranteed;
    }

    std::filesystem::path ownedRoot_;
};

TEST_F(ReaderTests, ReadShaderTest)
{
    // Arrange
    auto context = std::make_shared<context::OpenGLShaderContext>();
    context->setShaderPath("test-datas/shaders/vertex/good/minimal.vert");
    std::string code = R"(#version 330 core

uniform int testUniform;
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos, 1.0) * float(testUniform);
}
)";

    // Act
    shader::OpenGLShaderReader<Classic>::on(*context);

    // Assert
    ASSERT_EQ(context->getShaderCode(), code);
}

// F8 (mission C, ruling G-C2): an impossible opening is an IO failure, not a
// silent empty source. The component must reject with the public read error
// ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ and must not publish shader code
// that a later compilation would turn into an unrelated GL failure.
TEST_F(ReaderTests, ReadShaderTest_AbsentFileRejected)
{
    // Arrange: a guaranteed-absent file inside the owned root; the setup
    // precondition stops the test before any Reader call when the guarantee
    // cannot be established (H-TEMP-LOCK diagnosis, never a waived residue).
    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    const auto absentPath = absentTempShaderPath("absent-shader.vert");
    ASSERT_TRUE(absentPath.has_value())
        << "setup: a guaranteed-absent path is required for this test (see the setup diagnosis above)";
    shaderContext->setShaderPath(absentPath->string());

    // Act & Assert: the rejection keeps the read error nature.
    expectSpecificError(
        [&shaderContext]()
        { shader::OpenGLShaderReader<Classic>::on(*shaderContext); },
        TraceableException<std::runtime_error>("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ"));

    // The failed read must not leave anything to compile.
    ASSERT_TRUE(shaderContext->getShaderCode().empty());
}

// F8: an empty but readable file is a successful read of an empty source. The
// GL side then reports its own context (e.g. COMPILATION_FAILED), which is the
// correct nature for a present and readable but empty file.
TEST_F(ReaderTests, ReadShaderTest_EmptyReadableFileYieldsEmptyCode)
{
    // Arrange: an existing, readable, zero-byte file.
    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    const auto emptyPath = makeTempShaderSource("empty-shader.vert", "");
    ASSERT_TRUE(emptyPath.has_value())
        << "setup: the empty source fixture file is required (see the setup diagnosis above)";
    shaderContext->setShaderPath(emptyPath->string());
    ASSERT_TRUE(std::filesystem::exists(*emptyPath));

    // Act: the read must succeed.
    ASSERT_NO_THROW(shader::OpenGLShaderReader<Classic>::on(*shaderContext));

    // Assert: an empty code is published for the loader (GL decides next).
    ASSERT_EQ(shaderContext->getShaderCode(), "");
}

// F8: several read()/gcount() blocks must be concatenated without loss, so the
// transfer no longer relies on the streambuf hand-off (rdbuf) whose failure
// states stayed invisible. The size below spans multiple blocks and ends with
// a short final block (accepted with failbit+eofbit per the ruling).
TEST_F(ReaderTests, ReadShaderTest_MultiBlockReadPreservesContent)
{
    // Arrange: content spread over several extraction calls (> multiple
    // typical read buffers, final short block of 37 bytes).
    constexpr std::size_t contentSize = 3 * 4096 + 37;
    std::string expected(static_cast<std::size_t>(contentSize), '\0');
    for (std::size_t i = 0; i < contentSize; ++i)
    {
        expected[i] = static_cast<char>('A' + (i % 26));
    }

    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    const auto path = makeTempShaderSource("multi-block-shader.vert", expected);
    ASSERT_TRUE(path.has_value())
        << "setup: the multi-block source fixture file is required (see the setup diagnosis above)";
    shaderContext->setShaderPath(path->string());

    // Act
    ASSERT_NO_THROW(shader::OpenGLShaderReader<Classic>::on(*shaderContext));

    // Assert: byte-identical content (no whitespace swallowing, no truncation).
    ASSERT_EQ(shaderContext->getShaderCode(), expected);
}

// F8-SEAM cycle 2 characterization: exactly one full 4096-byte block, the
// loop then hitting end-of-file on the next read() (failbit+eofbit, zero
// additional bytes appended). Canonical success boundary of the read/gcount
// loop, characterized on the pre-seam on() before the extraction refactoring.
TEST_F(ReaderTests, ReadShaderTest_ExactlyOneBlockPreservesContent)
{
    constexpr std::size_t contentSize = 4096;
    std::string expected(static_cast<std::size_t>(contentSize), '\0');
    for (std::size_t i = 0; i < contentSize; ++i)
    {
        expected[i] = static_cast<char>('A' + (i % 26));
    }

    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    const auto path = makeTempShaderSource("exact-block-shader.vert", expected);
    ASSERT_TRUE(path.has_value())
        << "setup: the exact-block source fixture file is required (see the setup diagnosis above)";
    shaderContext->setShaderPath(path->string());

    // Act
    ASSERT_NO_THROW(shader::OpenGLShaderReader<Classic>::on(*shaderContext));

    // Assert: byte-identical content at the single-block boundary.
    ASSERT_EQ(shaderContext->getShaderCode(), expected);
}

// F8-SEAM cycle 1 (H-TEMP-LOCK RED observed, GREEN): a non-empty directory at
// the requested path cannot be removed by std::filesystem::remove; the
// pre-F8-SEAM helper ignored that failure and handed the surviving path back
// as if absent - the exact mechanism that turned a transient external lock
// into ReadShaderTest_AbsentFileRejected failing as "expected an exception".
// The guarantee must be enforced, never waived.
TEST_F(ReaderTests, AbsentPathSetup_RejectsUnremovableResidue)
{
    // Arrange: a non-empty directory occupies the requested-absent path.
    ASSERT_FALSE(ownedRoot_.empty())
        << "setup: the owned fixture root is required (see the setup diagnosis above)";
    const auto residue = ownedRoot_ / "residue-absent-shader.vert";
    std::error_code setupError;
    ASSERT_TRUE(std::filesystem::create_directories(residue / "occupied", setupError))
        << "setup: residue placement failed: " << setupError.message();
    ASSERT_TRUE(std::filesystem::exists(residue));

    // Act: the guarantee must either hold or reject the setup.
    const auto guaranteed = tryGuaranteeAbsentPath(ownedRoot_, "residue-absent-shader.vert");

    // Assert: the setup violation is reported (no path handed over), and the
    // residue stays in place for diagnosis (bounded cleanup removes it later).
    EXPECT_FALSE(guaranteed.has_value());
    EXPECT_TRUE(std::filesystem::exists(residue));
}

// F8-SEAM cycle 1: two fixture instances (or suites) must coexist. A second
// acquisition under the same parent receives its OWN directory -
// create_directory's atomic success is the exclusivity proof, a collision is
// treated by a fresh suffix, never by sharing a directory.
TEST_F(ReaderTests, OwnedRootSetup_AcquiresDistinctRootsPerFixtureInstance)
{
    ASSERT_FALSE(ownedRoot_.empty())
        << "setup: the owned fixture root is required (see the setup diagnosis above)";
    const auto secondRoot = tryAcquireOwnedRoot(readerFixturesParent());
    ASSERT_TRUE(secondRoot.has_value())
        << "setup: the second owned root acquisition failed (see its diagnosis)";

    std::error_code ec;
    EXPECT_NE(std::filesystem::weakly_canonical(*secondRoot, ec),
              std::filesystem::weakly_canonical(ownedRoot_, ec));
    EXPECT_TRUE(std::filesystem::exists(*secondRoot));

    // Each owner cleans only its own root.
    ASSERT_TRUE(tryCleanupOwnedRoot(*secondRoot, ec));
    EXPECT_FALSE(std::filesystem::exists(*secondRoot));
    EXPECT_TRUE(std::filesystem::exists(ownedRoot_));
}

// F8-SEAM isolation (finding f8a18a8): the owned root must NOT live under the
// legacy shared parent %TEMP%/artist-reader-tests. Older worktrees still
// running the pre-F8-SEAM fixture tear that whole parent down with
// std::filesystem::remove_all in their TearDown, which would destroy this
// suite's active owned roots mid-run: a unique child name does not protect
// against that wholesale cleanup. Direct path check - no cleanup is simulated
// against the real legacy directory.
TEST_F(ReaderTests, OwnedRootSetup_IsIsolatedFromLegacySharedParent)
{
    ASSERT_FALSE(ownedRoot_.empty())
        << "setup: the owned fixture root is required (see the setup diagnosis above)";
    const auto legacyParent = std::filesystem::temp_directory_path() / "artist-reader-tests";
    const auto canonicalRoot = std::filesystem::weakly_canonical(ownedRoot_);
    const auto canonicalLegacy = std::filesystem::weakly_canonical(legacyParent);

    // The acquired root must not have the legacy shared parent as an ancestor.
    EXPECT_FALSE(isAncestorPath(canonicalLegacy, canonicalRoot))
        << "the owned root " << canonicalRoot.string()
        << " lives under the legacy shared parent " << canonicalLegacy.string()
        << " that older worktrees remove wholesale in their TearDown";
}

// F8-SEAM cycle 1: cleanup must stay bounded to the owned root. The
// pre-F8-SEAM teardown removed the whole shared %TEMP%/artist-reader-tests
// tree, destroying any concurrent suite; a neighbor root planted next to ours
// must survive exactly the cleanup TearDown runs.
TEST_F(ReaderTests, Cleanup_IsBoundedToOwnedRootAndPreservesNeighbor)
{
    ASSERT_FALSE(ownedRoot_.empty())
        << "setup: the owned fixture root is required (see the setup diagnosis above)";
    const auto neighbor = tryAcquireOwnedRoot(readerFixturesParent());
    ASSERT_TRUE(neighbor.has_value())
        << "setup: the neighbor root acquisition failed (see its diagnosis)";
    const auto neighborFile = *neighbor / "neighbor-sentinel.vert";
    {
        std::ofstream neighborStream(neighborFile, std::ios::binary | std::ios::trunc);
        ASSERT_TRUE(neighborStream.is_open());
        neighborStream << "// neighbor sentinel\n";
        neighborStream.close();
        ASSERT_FALSE(neighborStream.fail());
    }
    ASSERT_TRUE(std::filesystem::exists(neighborFile));

    // Act: our cleanup, exactly what TearDown runs.
    std::error_code ec;
    ASSERT_TRUE(tryCleanupOwnedRoot(ownedRoot_, ec));

    // Assert: ours is gone, the neighbor is untouched.
    EXPECT_FALSE(std::filesystem::exists(ownedRoot_));
    EXPECT_TRUE(std::filesystem::exists(neighborFile));

    // The neighbor's own owner cleans it, bounded the same way.
    ASSERT_TRUE(tryCleanupOwnedRoot(*neighbor, ec));
    EXPECT_FALSE(std::filesystem::exists(*neighbor));
}

// F8-SEAM cycle 2: the real badbit failure branch, previously unexercised
// (docs/development/known-incidents.md, "F8 - preuve de lecture incomplete").
// The test adapter receives the real opened file from the transaction, then
// calls the SAME extractor the production path uses, with a local failing
// streambuf under the same badbit mask: first xsgetn serves exactly 4096
// bytes (the ruled block), the second request is counted, then a real
// ios_base::failure sentinel escapes the streambuf. The transaction must
// translate it into the public read error with the EXACT sentinel message
// (computed on the current STL), leave the pre-filled context empty and
// publish no partial block. GREEN by construction on the current catch: this
// is a seam-enablement proof, not an invented engine RED.
TEST_F(ReaderTests, ExtractionFailure_SentinelTranslatedWithExactMessage)
{
    // The expected message is computed on the current STL: the sentinel is a
    // real std::ios_base::failure and the translation keeps its what()
    // verbatim (MSVC 14.44 rethrows the original exception after setting
    // badbit, so the object that reaches the translation IS the sentinel).
    const std::ios_base::failure sentinel("reader-test-extraction-sentinel");
    const std::string expectedMessage =
        std::string("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n") + sentinel.what();

    constexpr std::size_t contentSize = 4096 + 512;
    std::string content(static_cast<std::size_t>(contentSize), '\0');
    for (std::size_t i = 0; i < contentSize; ++i)
    {
        content[i] = static_cast<char>('a' + (i % 26));
    }

    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    const auto path = makeTempShaderSource("sentinel-shader.vert", content);
    ASSERT_TRUE(path.has_value())
        << "setup: the sentinel source fixture file is required (see the setup diagnosis above)";
    shaderContext->setShaderPath(path->string());
    shaderContext->setShaderCode("PRE-FILLED-AND-MUST-BE-CLEARED");

    ScriptedStreambuf failing([]()
                              { throw std::ios_base::failure("reader-test-extraction-sentinel"); });
    scriptedStreambuf = &failing;

    // Act & Assert: exact final type and exact message (not merely a prefix).
    try
    {
        shader::detail::readShaderFile(*shaderContext, scriptedExtraction);
        FAIL() << "the injected extraction failure must be translated into the public read error";
    }
    catch (const TraceableException<std::runtime_error> &e)
    {
        EXPECT_EQ(std::string(e.what()), expectedMessage);
    }
    catch (...)
    {
        FAIL() << "unexpected exception type: the sentinel must be translated into "
                  "TraceableException<std::runtime_error>";
    }
    scriptedStreambuf = nullptr;

    // The first block was consumed (the real loop came back for more) with
    // the ruled 4096-byte request, the second request happened, and the
    // failure crossed the real loop as badbit (verified inside the adapter
    // while the stream lived).
    EXPECT_EQ(failing.requests(), 2);
    EXPECT_EQ(failing.firstRequestBytes(), 4096);

    // Context cleared by the transaction, no partial block published.
    EXPECT_TRUE(shaderContext->getShaderCode().empty());
}

// F8-SEAM cycle 2: a non-ios exception injected at the streambuf level
// (process-level bad_alloc) must NOT be remapped as a file error - it keeps
// its nature and the transaction leaves the context empty (cleared, never
// published). The current STL rethrows the original exception after setting
// badbit, which this test observes through the adapter.
TEST_F(ReaderTests, ExtractionFailure_BadAllocStaysUntranslated)
{
    constexpr std::size_t contentSize = 4096 + 512;
    std::string content(static_cast<std::size_t>(contentSize), '\0');
    for (std::size_t i = 0; i < contentSize; ++i)
    {
        content[i] = static_cast<char>('a' + (i % 26));
    }

    auto shaderContext = std::make_shared<context::OpenGLShaderContext>();
    const auto path = makeTempShaderSource("bad-alloc-shader.vert", content);
    ASSERT_TRUE(path.has_value())
        << "setup: the bad-alloc source fixture file is required (see the setup diagnosis above)";
    shaderContext->setShaderPath(path->string());
    shaderContext->setShaderCode("PRE-FILLED-AND-MUST-BE-CLEARED");

    ScriptedStreambuf failing([]()
                              { throw std::bad_alloc(); });
    scriptedStreambuf = &failing;

    // A remap into the business error would be caught here as a type mismatch.
    ASSERT_THROW(shader::detail::readShaderFile(*shaderContext, scriptedExtraction),
                 std::bad_alloc);
    scriptedStreambuf = nullptr;

    EXPECT_EQ(failing.requests(), 2);
    EXPECT_EQ(failing.firstRequestBytes(), 4096);
    EXPECT_TRUE(shaderContext->getShaderCode().empty());
}

// F8-SEAM cycle 2: closure happens before publication. The virtual test
// setter runs exactly at publication and attempts to remove the source file:
// on Windows the reader opens it without FILE_SHARE_DELETE, so a still-open
// handle would deny deletion - a successful remove at that exact point is the
// closure proof. The observation is only valid while the transaction lives.
// No close hook is added to production code.
TEST_F(ReaderTests, CloseHappensBeforePublication_ObservableThroughVirtualSetter)
{
    constexpr std::size_t contentSize = 2 * 4096 + 11;
    std::string expected(static_cast<std::size_t>(contentSize), '\0');
    for (std::size_t i = 0; i < contentSize; ++i)
    {
        expected[i] = static_cast<char>('A' + (i % 26));
    }

    auto shaderContext = std::make_shared<PublicationProbeContext>(expected);
    const auto path = makeTempShaderSource("close-before-publish-shader.vert", expected);
    ASSERT_TRUE(path.has_value())
        << "setup: the close-probe source fixture file is required (see the setup diagnosis above)";
    shaderContext->setShaderPath(path->string());

    // Act
    ASSERT_NO_THROW(shader::OpenGLShaderReader<Classic>::on(*shaderContext));

    // Assert: publication content intact, and the file handle was already
    // released when the virtual setter ran (the remove succeeded).
    ASSERT_EQ(shaderContext->getShaderCode(), expected);
    EXPECT_TRUE(shaderContext->removedAtPublication_)
        << "the filebuf borrowed during extraction was still open at publication: "
        << shaderContext->removalDiagnosis_;
}

#endif // __mock_gl__
