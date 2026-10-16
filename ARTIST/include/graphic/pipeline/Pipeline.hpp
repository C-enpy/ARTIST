#pragma once

#include <vector>
#include <string>
#include <format>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <utility>
#include <utils.hpp>
#include <common/exception/TraceableException.hpp>
#include <graphic/pipeline/Pass.hpp>
#include <graphic/context/PipelineContext.hpp>
#include <graphic/validator/ComponentConcept.hpp>

namespace artist::graphic::pipeline
{
    /**
     * @brief Interface for a generic rendering pipeline.
     *
     * This class template defines the foundation of a rendering pipeline, designed to work
     * with any graphics API by utilizing a templated API parameter. It manages a collection
     * of passes and a context unique to the specified API, enabling the construction and
     * execution of complex rendering tasks.
     *
     * @tparam API The graphics API specific context and functionality.
     */
    template <typename API>
    class IPipeline
    {
    public:
        // The context owns the pass registry. Destruction must never dispatch
        // an API hook from this base; the context destroys its registered passes.
        virtual ~IPipeline() = default;

        IPipeline(const IPipeline &) = delete;
        IPipeline &operator=(const IPipeline &) = delete;

        IPipeline(IPipeline &&other) : m_context(std::move(other.m_context)) {}

        /**
         * Constructs a pipeline with a list of passes and an optional context.
         *
         * @param passes A list of shared pointers to IPass objects to be added to the pipeline.
         * @param context The API-specific context whose ownership is transferred.
         */
        IPipeline(const std::initializer_list<std::shared_ptr<IPass<API>>> &passes,
                  std::unique_ptr<typename API::PipelineContext> context)
            : m_context(std::move(context))
        {
            if (!m_context)
            {
                throw common::exception::TraceableException<std::invalid_argument>("ERROR::PIPELINE::NULL_CONTEXT");
            }
            for (auto &pass : passes)
            {
                m_context->addPass(pass);
            }
        }

        explicit IPipeline(const std::initializer_list<std::shared_ptr<IPass<API>>> &passes) : IPipeline(passes,
                                                                                                         std::make_unique<typename API::PipelineContext>())
        {
        }

        /**
         * Retrieves a pass by its index.
         *
         * @param pass The index of the pass to retrieve.
         * @return A shared pointer to the requested IPass instance.
         */
        [[nodiscard]] std::shared_ptr<IPass<API>> forPass(const int &pass)
        {
            return getContext().getPass(pass);
        }

        /**
         * @return The total number of passes in the pipeline.
         */
        [[nodiscard]] virtual int getPassesCount() const
        {
            return getContext().getPassesCount();
        }

        /**
         * Checks if there are more passes to process.
         *
         * @return True if there are more passes, false otherwise.
         */
        [[nodiscard]] bool hasNext() const
        {
            return getContext().getCurrentPass() + 1 < getPassesCount();
        }

        /**
         * Advances the pipeline to use the next pass.
         *
         * @return True if there are remaining passes after the current one, false if the pipeline has completed.
         */
        virtual bool useNext()
        {
            if (hasNext())
            {
                use(getContext().getCurrentPass() + 1);
                return hasNext();
            }
            reset();
            return false;
        }

        /**
         * Activates a specific pass in the pipeline.
         *
         * @param pass The index of the pass to activate.
         */
        virtual void use(const int &pass)
        {
            auto &context = getContext();
            context.setCurrentPass(pass);
            use(context);
        }

        /**
         * Resets the pipeline to its initial state.
         * Resets the CPU cursor; this does not unbind a backend program.
         */
        virtual void reset()
        {
            reset(getContext());
        }

        /**
         * @return A borrow of the owned context, never a lifetime extension.
         * A move keeps its address; assignment invalidates destination borrows.
         * Callers serialize access and keep the required GPU context alive while
         * destroying passes. CPU scope alone does not prove GPU completion.
         */
        [[nodiscard]] virtual typename API::PipelineContext &getContext() const
        {
            if (!m_context)
            {
                throw common::exception::TraceableException<std::runtime_error>("ERROR::PIPELINE::FREED_CONTEXT");
            }
            return *m_context;
        }

    protected:
        IPipeline &operator=(IPipeline &&other)
        {
            if (this != &other)
            {
                // Destroy destination passes before installing the source context.
                m_context.reset();
                m_context = std::move(other.m_context);
            }
            return *this;
        }

        virtual void use(typename API::PipelineContext &context) = 0;
        virtual void reset(typename API::PipelineContext &context) = 0;

    private:
        std::unique_ptr<typename API::PipelineContext> m_context;
    };

    /**
     * @brief Specialized pipeline template that validates and utilizes API and PROFILE specific components.
     *
     * This class extends IPipeline by incorporating compile-time validation of API and PROFILE specific components
     * and their requirements. It leverages C++20 concepts and the 'requires' keyword for validation, ensuring
     * that each component and its operations are compatible and available for the given API and PROFILE.
     *
     * @tparam API The graphics API context.
     * @tparam PROFILE The specific profile indicating the specialized implementation requirements.
     */
    template <typename API, auto PROFILE>
        requires(API::Validator::template validatePipeline<API, PROFILE>())
    class Pipeline : public IPipeline<API>
    {
    public:
        using IPipeline<API>::IPipeline;
        using IPipeline<API>::use;
        using IPipeline<API>::reset;

        Pipeline(const Pipeline &) = delete;
        Pipeline &operator=(const Pipeline &) = delete;
        Pipeline(Pipeline &&other) : IPipeline<API>(std::move(other)) {}
        Pipeline &operator=(Pipeline &&other)
        {
            IPipeline<API>::operator=(std::move(other));
            return *this;
        }

    protected:
        /**
         * Uses the specialized component for the current profile and API.
         * This method dynamically checks for the existence of a component specific to the PROFILE
         * and invokes its 'on' method if available. This allows for profile-specific behavior
         * to be executed, adapting the pipeline to various rendering requirements.
         *
         * @param context A borrow of the current API's PipelineContext.
         */
        void use(typename API::PipelineContext &context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::PipelineContext::template User<PROFILE>>)
            {
                API::PipelineContext::template User<PROFILE>::on(context);
            }
        }

        /**
         * Resets the specialized component for the current profile and API.
         * Similar to the 'use' method, this checks for a resetter component specific to the PROFILE
         * and invokes its 'on' method to reset any state or configuration back to its default.
         * This is crucial for ensuring that the pipeline can be correctly reinitialized or reused.
         *
         * @param context A borrow of the current API's PipelineContext.
         */
        void reset(typename API::PipelineContext &context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::PipelineContext::template Resetter<PROFILE>>)
            {
                API::PipelineContext::template Resetter<PROFILE>::on(context);
            }
        }
    };

} // namespace artist::graphic::pipeline
