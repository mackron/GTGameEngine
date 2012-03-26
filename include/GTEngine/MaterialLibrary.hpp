
#ifndef __GTEngine_MaterialLibrary_hpp_
#define __GTEngine_MaterialLibrary_hpp_

#include "Material.hpp"

namespace GTEngine
{
    /// Library class for materials.
    ///
    /// Materials are a many-instantiation resource. This means the library uses a Create/Delete system. Shared materials will need
    /// to be done manually.
    ///
    /// The reason a material is a many-instantiation resource is because they can sometimes be dynamic. An example is when two or
    /// more meshes use the same material, but may have a different transparency. This is most easily handled by simply having
    /// multiple-instantiations of each material.
    class MaterialLibrary
    {
    // Starup/Shutdown.
    public:

        /// Starts up the material library.
        /// @return True if the material library was started up successfully.
        static bool Startup();

        /// Shuts down the environment.
        static void Shutdown();


    // Create/Delete.
    public:

        /// Creates a new material from a file.
        /// @param fileName [in] The name of the material file to load.
        /// @return A pointer to the new material.
        ///
        /// @remarks
        ///     Delete the returned material with Delete(). The material library does not do memory management; materials must be
        ///     deleted manually.
        static Material* Create(const char* fileName);

        /// Deletes a material created with Create().
        /// @param materialToDelete [in] A pointer to the material being deleted.
        static void Delete(Material* materialToDelete);
    };
}

#endif