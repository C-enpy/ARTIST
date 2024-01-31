#pragma once

#include <vector>
#include <string>
#include <format>
#include <iostream>
#include <unordered_map>
#include <memory>
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
        /**
         * Constructs a pipeline with a list of passes and an optional context.
         *
         * @param passes A list of shared pointers to IPass objects to be added to the pipeline.
         * @param context A shared pointer to the API-specific PipelineContext, defaulting to a new instance if not provided.
         */
        IPipeline(const std::initializer_list<std::shared_ptr<IPass<API>>> &passes,
                  std::shared_ptr<typename API::PipelineContext> context)
            : m_context(context)
        {
            for (auto &pass : passes)
            {
                m_context->addPass(pass);
            }
        }

        explicit IPipeline(const std::initializer_list<std::shared_ptr<IPass<API>>> &passes) : IPipeline(passes,
                                                                                                         std::make_shared<typename API::PipelineContext>())
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
            return m_context->getPass(pass);
        }

        /**
         * @return The total number of passes in the pipeline.
         */
        [[nodiscard]] virtual int getPassesCount() const
        {
            return m_context->getPassesCount();
        }

        /**
         * Checks if there are more passes to process.
         *
         * @return True if there are more passes, false otherwise.
         */
        [[nodiscard]] bool hasNext() const
        {
            return m_context->getCurrentPass() + 1 < getPassesCount();
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
                use(m_context->getCurrentPass() + 1);
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
            m_context->setCurrentPass(pass);
            use(m_context);
        }

        /**
         * Resets the pipeline to its initial state.
         * This is API specific and may involve unsetting the pipeline context.
         */
        virtual void reset()
        {
            reset(m_context);
        }

        /**
         * @return The current context of the pipeline.
         */
        [[nodiscard]] virtual std::shared_ptr<typename API::PipelineContext> getContext() const
        {
            return m_context;
        }

    protected:
        virtual void use(std::shared_ptr<typename API::PipelineContext> context) = 0;
        virtual void reset(std::shared_ptr<typename API::PipelineContext> context) = 0;

    private:
        std::shared_ptr<typename API::PipelineContext> m_context;
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

    protected:
        /**
         * Uses the specialized component for the current profile and API.
         * This method dynamically checks for the existence of a component specific to the PROFILE
         * and invokes its 'on' method if available. This allows for profile-specific behavior
         * to be executed, adapting the pipeline to various rendering requirements.
         *
         * @param context The shared pointer to the current API's PipelineContext.
         */
        void use(std::shared_ptr<typename API::PipelineContext> context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::PipelineContext::User<PROFILE>>)
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
         * @param context The shared pointer to the current API's PipelineContext.
         */
        void reset(std::shared_ptr<typename API::PipelineContext> context) override
        {
            if constexpr (graphic::validator::HasComponent<typename API::PipelineContext::Resetter<PROFILE>>)
            {
                API::PipelineContext::template Resetter<PROFILE>::on(context);
            }
        }
    };

} // namespace artist::graphic::pipeline
