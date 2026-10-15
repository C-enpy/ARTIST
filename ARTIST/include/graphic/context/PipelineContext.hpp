// PipelineContext.hpp

#pragma once

#include <memory>
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
        virtual ~PipelineContext() = default;

        void addPass(std::shared_ptr<pipeline::IPass<API>> pass)
        {
            m_passes.push_back(pass);
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
         * registered shared_ptr; a rejection leaves the context untouched.
         */
        [[nodiscard]] std::shared_ptr<pipeline::IPass<API>> getPass(const int &index) const
        {
            if (index < 0 || static_cast<decltype(m_passes)::size_type>(index) >= m_passes.size())
            {
                throw common::exception::TraceableException<std::out_of_range>(std::format("ERROR::PIPELINE::PASS_INDEX_OUT_OF_RANGE\nPass index {} out of range (size {})", index, m_passes.size()));
            }
            return m_passes[static_cast<decltype(m_passes)::size_type>(index)];
        }

        [[nodiscard]] unsigned int getPassesCount() const
        {
            return m_passes.size();
        }

        void setCurrentPass(const int &pass)
        {
            m_currentPass = pass;
        }

        [[nodiscard]] int getCurrentPass() const
        {
            return m_currentPass;
        }

    private:
        std::vector<std::shared_ptr<pipeline::IPass<API>>> m_passes;
        int m_currentPass = -1;
    };
}