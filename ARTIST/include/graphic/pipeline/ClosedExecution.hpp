/**
 * @file ClosedExecution.hpp
 * @brief Closed N=2 execution over two borrowed passes.
 *
 * The sequence length is structural: two reference members and one literal
 * call order. The passes, their contexts and any native program stay owned
 * by the caller. This type is not a pipeline and does not replace one.
 */

#pragma once

#include <graphic/pipeline/Pass.hpp>

namespace artist::graphic::pipeline
{
    /**
     * @brief Borrows two passes and activates them in order.
     *
     * Construction only binds the borrows. It does not probe pass validity,
     * read or write a pipeline cursor, or copy a program handle. run() calls
     * first.use() then second.use(). An exception leaves the function
     * immediately, so the later pass is not activated and the exception object
     * is unchanged. Backend effects are exactly those of IPass::use().
     *
     * Both owners must outlive this object. Destroying it does not free either
     * pass. A moved-from pass fails inside its own use(), with the diagnostic
     * that pass already raises.
     *
     * @tparam API Graphics API shared by the borrowed passes.
     */
    template <typename API>
    class ClosedExecution
    {
    public:
        ClosedExecution(IPass<API> &first, IPass<API> &second)
            : m_first(first), m_second(second)
        {
        }

        void run()
        {
            m_first.use();
            m_second.use();
        }

    private:
        /// Non-owning borrows, in activation order. Not a cursor and not a
        /// cached program handle.
        IPass<API> &m_first;
        IPass<API> &m_second;
    };
} // namespace artist::graphic::pipeline
