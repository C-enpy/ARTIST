// PipelineContext.hpp

#pragma once

#include <memory>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>
#include <common/exception/TraceableException.hpp>
#include <format>
#include <graphic/Api.hpp>
#include <graphic/pipeline/Pass.hpp>

namespace artist::graphic::context
{
    namespace pipeline = artist::graphic::pipeline;
    template <typename API>
    class PipelineContext
    {
    public:
        PipelineContext() = default;
        virtual ~PipelineContext() = default;
        PipelineContext(const PipelineContext &) = delete;
        PipelineContext &operator=(const PipelineContext &) = delete;
        PipelineContext(PipelineContext &&) = delete;
        PipelineContext &operator=(PipelineContext &&) = delete;

        // Isolated guard: test the cardinality boundary without allocating an
        // INT_MAX-sized registry. passCount is the count BEFORE insertion.
        static void validatePassCountForAddition(std::size_t passCount)
        {
            if (passCount >= static_cast<std::size_t>(std::numeric_limits<int>::max()))
            {
                throw common::exception::TraceableException<std::length_error>(std::format(
                    "ERROR::PIPELINE::PASS_COUNT_LIMIT_EXCEEDED\nPass count cannot exceed {}", std::numeric_limits<int>::max()));
            }
        }

        // Takes ownership even on failure. Existing entries, their addresses
        // and the cursor are unchanged if validation or allocation fails.
        void addPass(std::unique_ptr<pipeline::IPass<API>> pass)
        {
            if (!pass)
            {
                throw common::exception::TraceableException<std::invalid_argument>("ERROR::PIPELINE::NULL_PASS");
            }
            validatePassCountForAddition(m_passes.size());
            m_passes.push_back(std::move(pass));
        }

        /**
         * @brief Borrow the pass registered at the given index.
         * @param index Index of the pass to get.
         * @return Registered pass at the given index.
         *
         * F4 (mission C): out-of-range and negative accesses are rejected
         * with the business exception ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE
         * before any indexing; the negative check happens before the
         * size_type conversion so -1 cannot wrap into a huge offset, and the
         * range comparison itself is done in vector::size_type. vector::at
         * is intentionally not used. In-range returns keep handing the same
         * registered object; a rejection leaves the context untouched.
         * Adding passes can grow the vector, but does not move these objects.
         * Borrows do not extend lifetime; callers serialize all access.
         */
        [[nodiscard]] pipeline::IPass<API> &getPass(const int &index) const
        {
            if (index < 0 || static_cast<decltype(m_passes)::size_type>(index) >= m_passes.size())
            {
                throw common::exception::TraceableException<std::out_of_range>(std::format("ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE\nPass index {} out of range (size {})", index, m_passes.size()));
            }
            return *m_passes[static_cast<decltype(m_passes)::size_type>(index)];
        }

        [[nodiscard]] unsigned int getPassesCount() const
        {
            return static_cast<unsigned int>(m_passes.size());
        }

        void setCurrentPass(const int &pass)
        {
            if (pass != -1)
            {
                (void)getPass(pass);
            }
            m_currentPass = pass;
        }

        [[nodiscard]] int getCurrentPass() const
        {
            return m_currentPass;
        }

    private:
        std::vector<std::unique_ptr<pipeline::IPass<API>>> m_passes;
        int m_currentPass = -1;
    };
}
