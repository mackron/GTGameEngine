
#ifndef __GTEngine_ModelLibrary_hpp_
#define __GTEngine_ModelLibrary_hpp_

#include "Model.hpp"

namespace GTEngine
{
    /// Library class for models.
    ///
    /// Models are a many-instantiation resource, meaning we use a Create/Delete system.
    ///
    /// The reason Models are many-instantiation is because models can be in different animations states and also have different
    /// materials applied at a time. Geometric data is shared, and is managed by the library.
    class ModelLibrary
    {
    // Create functions.
    public:


        /// Loads a model from a file.
        static Model* LoadFromFile(const char* fileName);


        /**
        *   \brief  Creates a model from a collada file.
        */
        static Model* LoadFromCollada(const char* filename);

        /**
        *   \brief  Creates a flat plane on the XZ axis, facing the +Y axis.
        */
        static Model* CreatePlaneXZ(float width, float height, VertexFormat &format = VertexFormat::P3T2N3);



    // Delete functions.
    public:

        /**
        *   \brief  Deletes a model that was created with the model factory.
        */
        static void Delete(Model* model);
    };
}

#endif