#pragma once
#include <cstdint>
#include <limits>

namespace artist::graphic::context::detail
{
    // Identity/liveness only: never owns a resource, window or context.
    // Mutations and uses are serialized, non-reentrant and single-threaded.
    class PreparedUniformState
    {
    public:
        std::uint64_t generation() const { return m_generation; }
        bool accepts(std::uint64_t generation) const
        {
            return !m_retired && generation == m_generation;
        }
        void invalidate()
        {
            // Saturate permanently rather than rehabilitate an old access.
            if (m_generation == (std::numeric_limits<std::uint64_t>::max)())
                m_retired = true;
            else ++m_generation;
        }
    private:
        std::uint64_t m_generation = 0;
        bool m_retired = false;
    };
#ifdef __mock_gl__
    struct PreparedUniformObservations
    {
        static inline std::uint64_t nameSearches = 0;
        static inline std::uint64_t extractions = 0;
    };
#endif
}
