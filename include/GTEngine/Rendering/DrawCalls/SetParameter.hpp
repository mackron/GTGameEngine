
#ifndef __GTEngine_DrawCall_SetParameter_hpp_
#define __GTEngine_DrawCall_SetParameter_hpp_

#include "../RenderCommands.hpp"

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
#define GLM_FORCE_ONLY_XYZW
#include <glm/glm.hpp>
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

namespace GTEngine
{
    namespace DrawCalls
    {
        class SetParameter : public RenderCommand
        {
        public:

            SetParameter(const char *name);
            virtual ~SetParameter();


        protected:

            char *name;


        private:    // No coyping.
            SetParameter(const SetParameter &);
            SetParameter & operator=(const SetParameter &);
        };
    }
}

namespace GTEngine
{
    class Texture2D;

    namespace DrawCalls
    {
        class SetParameter_sampler2D : public SetParameter
        {
        public:

            SetParameter_sampler2D(const char *name, Texture2D *value);
            void Execute();
            void OnExecuted();


        public: // Allocation/Deallocation.

            static SetParameter_sampler2D * Allocate(const char *name, Texture2D *value)
            {
                return new SetParameter_sampler2D(name, value);
            }

            static void Deallocate(SetParameter_sampler2D *call)
            {
                delete call;
            }


        private:

            Texture2D *value;

        private:    // No copying.
            SetParameter_sampler2D(const SetParameter_sampler2D &);
            SetParameter_sampler2D & operator=(const SetParameter_sampler2D &);
        };


        class SetParameter_float : public SetParameter
        {
        public:

            SetParameter_float(const char *name, float value);
            void Execute();
            void OnExecuted();


        public: // Allocation/Deallocation.

            static SetParameter_float * Allocate(const char *name, float value)
            {
                return new SetParameter_float(name, value);
            }

            static void Deallocate(SetParameter_float *call)
            {
                delete call;
            }


        private:

            float value;
        };


        class SetParameter_float2 : public SetParameter
        {
        public:

            SetParameter_float2(const char *name, const glm::vec2 &value);
            void Execute();
            void OnExecuted();


        public: // Allocation/Deallocation.

            static SetParameter_float2 * Allocate(const char *name, const glm::vec2 &value)
            {
                return new SetParameter_float2(name, value);
            }

            static void Deallocate(SetParameter_float2 *call)
            {
                delete call;
            }


        private:

            glm::vec2 value;
        };


        class SetParameter_float3 : public SetParameter
        {
        public:

            SetParameter_float3(const char *name, const glm::vec3 &value);
            void Execute();
            void OnExecuted();


        public: // Allocation/Deallocation.

            static SetParameter_float3 * Allocate(const char *name, const glm::vec3 &value)
            {
                return new SetParameter_float3(name, value);
            }

            static void Deallocate(SetParameter_float3 *call)
            {
                delete call;
            }


        private:

            glm::vec3 value;
        };


        class SetParameter_float4 : public SetParameter
        {
        public:

            SetParameter_float4(const char *name, const glm::vec4 &value);
            void Execute();
            void OnExecuted();


        public: // Allocation/Deallocation.

            static SetParameter_float4 * Allocate(const char *name, const glm::vec4 &value)
            {
                return new SetParameter_float4(name, value);
            }

            static void Deallocate(SetParameter_float4 *call)
            {
                delete call;
            }


        private:

            glm::vec4 value;
        };



        class SetParameter_float2x2 : public SetParameter
        {
        public:

            SetParameter_float2x2(const char *name, const glm::mat2 &value);
            void Execute();
            void OnExecuted();


        public: // Allocation/Deallocation.

            static SetParameter_float2x2 * Allocate(const char *name, const glm::mat2 &value)
            {
                return new SetParameter_float2x2(name, value);
            }

            static void Deallocate(SetParameter_float2x2 *call)
            {
                delete call;
            }


        private:

            glm::mat2 value;
        };


        class SetParameter_float3x3 : public SetParameter
        {
        public:

            SetParameter_float3x3(const char *name, const glm::mat3 &value);
            void Execute();
            void OnExecuted();


        public: // Allocation/Deallocation.

            static SetParameter_float3x3 * Allocate(const char *name, const glm::mat3 &value)
            {
                return new SetParameter_float3x3(name, value);
            }

            static void Deallocate(SetParameter_float3x3 *call)
            {
                delete call;
            }


        private:

            glm::mat3 value;
        };


        class SetParameter_float4x4 : public SetParameter
        {
        public:

            SetParameter_float4x4(const char *name, const glm::mat4 &value);
            void Execute();
            void OnExecuted();


        public: // Allocation/Deallocation.

            static SetParameter_float4x4 * Allocate(const char *name, const glm::mat4 &value)
            {
                return new SetParameter_float4x4(name, value);
            }

            static void Deallocate(SetParameter_float4x4 *call)
            {
                delete call;
            }


        private:

            glm::mat4 value;
        };



    }
}

#endif