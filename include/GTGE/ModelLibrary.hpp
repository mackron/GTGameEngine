// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_ModelLibrary
#define GT_ModelLibrary

#include "Model.hpp"

namespace GT
{
    class Context;

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
    public:

        /// Constructor.
        ModelLibrary(Context &context);

        /// Destructor.
        ~ModelLibrary();



        ////////////////////////////////////////////////
        // Startup/Shutdown

        /// Starts up the model library.
        bool Startup();

        /// Shutsdown the model library.
        void Shutdown();


        ////////////////////////////////////////////////
        // Create/Delete

        /// Loads a model from a file.
        ///
        /// @param fileName       [in] The name of the file to load.
        /// @param makeRelativeTo [in] If 'fileName' is absolute, it will be made relative to this path.
        ///
        /// @return A pointer to the new model, or null if the file could not be loaded.
        ///
        /// @remarks
        ///     This will not reload the file if it has previously been loaded. Thus, calling this function with the same file will
        ///     be quick after the first load.
        Model* Create(const char* fileName, const char* makeRelativeTo = nullptr);

        /// Creates a model from a definition.
        ///
        /// @param definition [in] A reference to the model defintinitin to create the model from.
        ///
        /// @return A pointer to the new model if successful; null otherwise.
        Model* CreateFromDefinition(const ModelDefinition &definition);

        /// Creates a model from a convex-hull.
        ///
        /// @param convexHull [in] A reference to the convexHull
        Model* CreateFromConvexHull(const ConvexHull &convexHull);


        /// Deletes a model that was created with the model library.
        ///
        /// @oaram model [in] A reference to the model to delete.
        void Delete(Model* model);
        void Delete(Model &model) { ModelLibrary::Delete(&model); }


        /// Reloads the given model. If the model has not yet been loaded, it will NOT be loaded.
        ///
        /// @param fileName [in] The name of the file to reload.
        ///
        /// @remarks
        ///     This will update every model using the given file.
        bool Reload(const char* fileName);



        ////////////////////////////////////////////////
        // Writing

        /// Saves the givem model as a .gtmodel.
        ///
        /// @param model    [in] A reference to the model to save.
        /// @param fileName [in] the file name of the model.
        ///
        /// @return True if the model is saved successfully; false otherwise.
        ///
        /// @remarks
        ///     This will overwrite any existing file without warning.
        ///     @par
        ///     If <fileName> does not have the .gtmodel extension, it will be appended on.
        ///     @par
        ///     This function does not save the model as any format other than .gtmodel.
        bool WriteToFile(const ModelDefinition &definition, const char* fileName);
        bool WriteToFile(const ModelDefinition &definition);
        bool WriteToFile(const char* fileName);



        ////////////////////////////////////////////////
        // Misc.

        /// Determines whether or not the given file extension is supported.
        ///
        /// @remarks
        ///     If this function returns true, it does not necessarily mean it can load every file of that extension.
        static bool IsExtensionSupported(const char* extension);



    /////////////////////////////////////////////////////
    // Private Functions.
    private:

        /// Finds the absolute path of the given model file.
        ///
        /// @param relativePath [in ] The relative path whose absolute path is being found.
        /// @param absolutePath [out] A reference to the string that will receive the absolute path.
        ///
        /// @return True if the file exists and 'absolutePath' contains a valid path.
        bool FindAbsolutePath(const char* relativePath, String &absolutePath);

        /// Retrieves the model definition of the given file name.
        ///
        /// @param fileName [in] The name of the file whose definition is being retrieved.
        ///
        /// @return A pointer to the definition, or null if does not exist.
        ModelDefinition* FindDefinition(const char* fileName);

        /// Creates a model from a primitive's vertex array.
        ///
        /// @param name [in] The name of the primitive.
        /// @param va   [in] The vertex array of the primitive. This can be nullptr, but only if the primitive has already been created.
        Model* CreateFromPrimitive(const char* name, VertexArray* va);


    private:

        /// A reference to the context that owns this library.
        Context &m_context;


        struct ModelDefinitionReference
        {
            ModelDefinition* definition;
            size_t referenceCount;

            ModelDefinitionReference(ModelDefinition* definitionIn, size_t referenceCountIn)
                : definition(definitionIn), referenceCount(referenceCountIn)
            {
            }

            ModelDefinitionReference(const ModelDefinitionReference &other)
                : definition(other.definition), referenceCount(other.referenceCount)
            {
            }

            ModelDefinitionReference & operator=(const ModelDefinitionReference &other)
            {
                this->definition     = other.definition;
                this->referenceCount = other.referenceCount;

                return *this;
            }
        };

        /// The list of loaded model definitions, index by the absolute path of the original source file.
        Dictionary<ModelDefinitionReference> m_loadedDefinitions;

        /// The list of instantiated models. we need this so we can delete them on shutdown.
        Vector<Model*> m_instantiatedModels;
    };
}

#endif