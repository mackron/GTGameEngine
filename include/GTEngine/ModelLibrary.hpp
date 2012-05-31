
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
    ///
    /// When models are loaded, the bind-pose mesh data remains in-memory
    class ModelLibrary
    {
    // Startup/Shutdown.
    public:

        /// Starts up the model library.
        static bool Startup();

        /// Shutsdown the model library.
        static void Shutdown();


    // Create functions.
    public:

        /// Loads a model from a file.
        ///
        /// @param fileName [in] The name of the file to load.
        ///
        /// @return A pointer to the new model, or null if the file could not be loaded.
        ///
        /// @remarks
        ///     This will not reload the file if it has previously been loaded. Thus, calling this function with the same file will
        ///     be quick after the first load.
        static Model* LoadFromFile(const char* fileName);

        /// Generically loads a file from Neutral File Format (NFF) content.
        ///
        /// @param content [in] The content of the NFF file.
        /// @param name    [in] The name to use for uniquely identifying this NFF. This will usually be based on the format of the model.
        ///
        /// @return A pointer to the new model, or null if the model could not be created.
        ///
        /// @remarks
        ///     This will not reload the model if the same content has been used before.
        static Model* LoadFromNFF(const char* content, const char* name);



        /// Creates a flat plane on the XZ axis, facing the +Y axis.
        static Model* CreatePlaneXZ(float width, float height, VertexFormat &format = VertexFormat::P3T2N3);

        /// Creates a box.
        ///
        /// @remarks
        ///     The box will use the P3T2N3T3B3 vertex format.
        static Model* CreateBox(float halfWidth, float halfHeight, float halfDepth);

        /// Creates a sphere.
        ///
        /// @radius [in] The radius of the sphere.
        static Model* CreateSphere(float radius);

        /// Creates a cylinder, standing up on the y axis.
        ///
        /// @param radius [in] The radius of the cylinder.
        /// @param length [in] The length of the cylinder.
        static Model* CreateCylinder(float radius, float length);



    // Delete functions.
    public:

        /// Deletes a model that was created with the model library.
        static void Delete(Model* model);
    };
}

#endif