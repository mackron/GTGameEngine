// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_MaterialLibrary
#define GT_MaterialLibrary

#include "Material.hpp"

namespace GT
{
    class Context;

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
    public:

        /// Constructor.
        MaterialLibrary(Context &context);

        /// Destructor.
        ~MaterialLibrary();


        /// Starts up the material library.
        ///
        /// @return True if the material library was started up successfully.
        bool Startup();

        /// Shuts down the environment.
        void Shutdown();


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
        void AttachEventHandler(EventHandler &handler);

        /// Removes the given event handler from the material library.
        ///
        /// @param handler [in] A reference to the event handler to remove from the material library.
        void RemoveEventHandler(EventHandler &handler);



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
        Material* Create(const char* fileName, const char* makeRelativeTo = nullptr);

        /// Creates a copy of the given material.
        ///
        /// @param source [in] A reference to the material to copy.
        ///
        /// @remarks
        ///     A pointer to the new material, or null if an error occurs.
        Material* CreateCopy(const Material &source);


        /// Deletes a material created with Create().
        ///
        /// @param materialToDelete [in] A pointer to the material being deleted.
        void Delete(Material* materialToDelete);


        /// Reloads the given material. If the material has not yet been loaded, it will NOT be loaded.
        ///
        /// @param fileName [in] The name of the file to reload.
        ///
        /// @remarks
        ///     This will update every material using the given file.
        bool Reload(const char* fileName);


        /// Creates a material for use with navigation meshes.
        Material* CreateNavigationMeshMaterial();




    private:

        /// Helper function for calling the OnCreateMaterial() event.
        void OnCreateMaterial(Material &material);

        /// Helper function for calling the OnDeleteMaterial() event.
        void OnDeleteMaterial(Material &material);

        /// Helper function for calling the OnCreateMaterialDefinition() event.
        void OnCreateMaterialDefinition(MaterialDefinition &definition);

        /// Helper function for calling the OnDeleteMaterialDefinition() event.
        void OnDeleteMaterialDefinition(MaterialDefinition &definition);

        /// Helper function for calling the OnReloadMaterialDefinition() event.
        void OnReloadMaterialDefinition(MaterialDefinition &definition);



    private:

        /// A reference to the context that owns this library.
        Context &m_context;


        struct MaterialDefinitionReference
        {
            MaterialDefinition* definition;
            size_t referenceCount;

            MaterialDefinitionReference(MaterialDefinition* definitionIn, size_t referenceCountIn)
                : definition(definitionIn), referenceCount(referenceCountIn)
            {
            }

            MaterialDefinitionReference(const MaterialDefinitionReference &other)
                : definition(other.definition), referenceCount(other.referenceCount)
            {
            }


            MaterialDefinitionReference & operator=(const MaterialDefinitionReference &other)
            {
                this->definition     = other.definition;
                this->referenceCount = other.referenceCount;

                return *this;
            }
        };


        /// The list of loaded material definitions, indexed by their absolute path.
        Dictionary<MaterialDefinitionReference> m_materialDefinitions;

        /// The list of loaded materials.
        List<Material*> m_loadedMaterials;


        /// The list of event handlers.
        Vector<MaterialLibrary::EventHandler*> m_eventHandlers;
    };
}

#endif