#include "WorkloadProtocol.hpp"
#include <graphic/context/UniformContext.hpp>

namespace
{
    // UniformContext's API parameter is unused by the existing CPU value store.
    // This local domain tag instantiates that template without creating a GPU
    // backend or claiming that the mirror scenario performs a Pass upload.
    struct CpuMirrorDomain {};
    using CpuMirror = artist::graphic::context::UniformContext<CpuMirrorDomain>;
}

#ifdef _WIN32
int wmain(int argc, wchar_t **argv)
#else
int main(int argc, char **argv)
#endif
{
    namespace verification = artist::verification;
    return verification::mainWithResult(argc, argv, false,
        [](const verification::Request &request, verification::Result &result)
        {
            // Existing CPU mirror API, no mocked GL and no context. The CLI
            // pass-name label is not evidence of a Pass upload in this scenario.
            std::vector<CpuMirror> mirrors(request.batch);
            constexpr std::array values{0.f, .5f, 0.f};
            std::uint64_t controlUpdates = 0;
            for (float value : values)
                for (auto &mirror : mirrors)
                {
                    mirror.setValue<float>(value);
                    ++controlUpdates;
                    verification::require(mirror.getValue<float>() == value, "CPU sequence oracle failed");
                }
            for (std::uint64_t repetition = 0; repetition < request.repetitions; ++repetition)
                for (std::uint64_t iteration = 0; iteration < request.iterations; ++iteration)
                {
                    const float value = values[(iteration + repetition) % values.size()];
                    for (auto &mirror : mirrors)
                    {
                        mirror.setValue<float>(value);
                        ++result.updates;
                        verification::require(mirror.getValue<float>() == value, "CPU mirror oracle failed");
                    }
                }
            result.observations = "{\"api\":\"UniformContext::setValue/getValue<float>\","
                "\"proves_upload\":false,\"control_sequence\":[0,0.5,0],\"control_updates\":"
                + std::to_string(controlUpdates) + ",\"counted_updates\":\"main population only\"}";
            result.environment = "{\"execution_domain\":\"cpu-only\",\"gl_context_created\":false}";
            result.verdict = "PASS";
        });
}
