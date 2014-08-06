// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

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
        ///
        /// @return True if the material library was started up successfully.
        static bool Startup();

        /// Shuts down the environment.
        static void Shutdown();


    // Event handlers.
    public:

        /// Class representing an event handler for the material library.
        class EventHandler
        {
        public:

            /// Constructor.
            EventHandler()
            {
            }

            /// Destructor.
            virtual ~EventHandler()
            {
            }


            /// Called just after a material is created.
            ///
            /// @param material [in] A reference to the material that was just created.
            virtual void OnCreateMaterial(Material &) {}

            /// Calld just before a material is deleted.
            ///
            /// @param material [in] A reference to the material that is about to be deleted.
            virtual void OnDeleteMaterial(Material &) {}

            /// Called just after a material definition is created for the first time.
            ///
            /// @param definition [in] A reference to the material definition that was just created.
            virtual void OnCreateMaterialDefinition(MaterialDefinition &) {}

            /// Called just before a material definition is deleted.
            ///
            /// @param definition [in] A reference to the material definition that is about to be deleted.
            virtual void OnDeleteMaterialDefinition(MaterialDefinition &) {}

            /// Called just after a material definition is reloaded.
            ///
            /// @param definition [in] A reference to the material definition that has just been reloaded.
            virtual void OnReloadMaterialDefinition(MaterialDefinition &) {}
        };


        /// Attaches an event handler to the material library.
        ///
        /// @param handler [in] A reference to the event handler to attach to the material library.
        ///
        /// @remarks
        ///     Use event handlers to catch material creation and deletion. Really useful for doing shader cleanup.
        static void AttachEventHandler(EventHandler &handler);

        /// Removes the given event handler from the material library.
        ///
        /// @param handler [in] A reference to the event handler to remove from the material library.
        static void RemoveEventHandler(EventHandler &handler);



    // Create/Delete.
    public:

        /// Creates a new material from a file.
        ///
        /// @param fileName [in] The name of the material file to load.
        ///
        /// @return A pointer to the new material.
        ///
        /// @remarks
        ///     Delete the returned material with Delete(). The material library does not do memory management; materials must be
        ///     deleted manually.
        ///
        ///     This will not reload the file if it has previously been loaded. Thus, calling this function with the same file will
        ///     be quick after the first load. Use Reload() to reload a file.
        static Material* Create(const char* fileName, const char* makeRelativeTo = nullptr);

        /// Creates a copy of the given material.
        ///
        /// @param source [in] A reference to the material to copy.
        ///
        /// @remarks
        ///     A pointer to the new material, or null if an error occurs.
        static Material* CreateCopy(const Material &source);


        /// Deletes a material created with Create().
        ///
        /// @param materialToDelete [in] A pointer to the material being deleted.
        static void Delete(Material* materialToDelete);


        /// Reloads the given material. If the material has not yet been loaded, it will NOT be loaded.
        ///
        /// @param fileName [in] The name of the file to reload.
        ///
        /// @remarks
        ///     This will update every material using the given file.
        static bool Reload(const char* fileName);


        /// Creates a material for use with navigation meshes.
        static Material* CreateNavigationMeshMaterial();
    };
}

#endif