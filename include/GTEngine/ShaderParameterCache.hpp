
#ifndef __GTEngine_ShaderParameterCache_hpp_
#define __GTEngine_ShaderParameterCache_hpp_

#include "ShaderParameter.hpp"
#include "Math.hpp"
#include <GTCore/Dictionary.hpp>

namespace GTEngine
{
    class Texture2D;

    /// Class representing a cache of shader parameters.
    class ShaderParameterCache
    {
    public:

        /// Constructor.
        ShaderParameterCache();

        /// Destructor.
        ~ShaderParameterCache();

        /// Setters.
        void Set(const char* name, float x);
        
        void Set(const char* name, const glm::vec2 &v);
        void Set(const char* name, const glm::vec3 &v);
        void Set(const char* name, const glm::vec4 &v);
        void Set(const char* name, float x, float y)                   { this->Set(name, glm::vec2(x, y)); }
        void Set(const char* name, float x, float y, float z)          { this->Set(name, glm::vec3(x, y, z)); }
        void Set(const char* name, float x, float y, float z, float w) { this->Set(name, glm::vec4(x, y, z, w)); }

        void Set(const char* name, const glm::mat2 &v);
        void Set(const char* name, const glm::mat3 &v);
        void Set(const char* name, const glm::mat4 &v);

        void Set(const char* name, Texture2D* texture);

        /// This overload makes a local copy of the input parameter.
        void Set(const char* name, const ShaderParameter* parameter);


        /// Retrieves a parameter by its name.
        /// @param name [in] The name of the parameter to retrieve.
        ShaderParameter* Get(const char* name) const;
        ShaderParameter* Get(const char* name);

        /// Retrieves a parameter by its index.
        /// @param index [in] The index of the parameter to retrieve. This is a O(1) direct accessor.
        ///
        /// @remarks
        ///     Use GetCount() to iterate over each parameter.
        ///     @par
        ///     This does not do run-time checks for index validity. If the index is invalid, an assert will fail.
        ShaderParameter* GetByIndex(size_t index) const;
        ShaderParameter* GetByIndex(size_t index);

        /// Retrieves the name of a parameter by it's index.
        /// @param index [in] The index of the parameter whose name is being retrieved. This is an O(1) direct accessor.
        const char* GetNameByIndex(size_t index) const;


        /// Retrieves the number of parameters in the cache.
        size_t GetCount() const;

        /// Retrieves a reference to the internal map of parameters.
        const GTCore::Dictionary<char, ShaderParameter*> & GetParameters() const { return this->parameters; }
              GTCore::Dictionary<char, ShaderParameter*> & GetParameters()       { return this->parameters; }

        /// Clears the cache.
        void Clear();


    private:

        /// Generically sets the value of a parameter. T is the type class (ShaderParameter_Float2, etc) and U is the value type (vec2, mat4, Texture2D, etc).
        template <typename T, typename U>
        void SetGeneric(const char* name, const U &value)
        {
            auto iParam = this->parameters.Find(name);
            if (iParam != nullptr)
            {
                auto param = T::Upcast(iParam->value);
                if (param != nullptr)
                {
                    param->value = value;
                }
                else
                {
                    delete iParam;
                    this->parameters.Add(name, new T(value));
                }
            }
            else
            {
                this->parameters.Add(name, new T(value));
            }
        }


    private:

        /// The dictionary containing pointers to ShaderParameter objects. The key is the parameter name.
        GTCore::Dictionary<char, ShaderParameter*> parameters;
    };
}


#endif