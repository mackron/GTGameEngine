// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ModelLibrary_hpp_
#define __GTEngine_ModelLibrary_hpp_

#include "Model.hpp"
#include <GTLib/IO.hpp>

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
    public:

        ////////////////////////////////////////////////
        // Startup/Shutdown

        /// Starts up the model library.
        static bool Startup();

        /// Shutsdown the model library.
        static void Shutdown();


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
        static Model* Create(const char* fileName, const char* makeRelativeTo = nullptr);

        /// Creates a model from a definition.
        ///
        /// @param definition [in] A reference to the model defintinitin to create the model from.
        ///
        /// @return A pointer to the new model if successful; null otherwise.
        static Model* CreateFromDefinition(const ModelDefinition &definition);

        /// Creates a model from a convex-hull.
        ///
        /// @param convexHull [in] A reference to the convexHull
        static Model* CreateFromConvexHull(const ConvexHull &convexHull);


        /// Deletes a model that was created with the model library.
        ///
        /// @oaram model [in] A reference to the model to delete.
        static void Delete(Model* model);
        static void Delete(Model &model) { ModelLibrary::Delete(&model); }


        /// Reloads the given model. If the model has not yet been loaded, it will NOT be loaded.
        ///
        /// @param fileName [in] The name of the file to reload.
        ///
        /// @remarks
        ///     This will update every model using the given file.
        static bool Reload(const char* fileName);



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
        static bool WriteToFile(const ModelDefinition &definition, const char* fileName);
        static bool WriteToFile(const ModelDefinition &definition);
        static bool WriteToFile(const char* fileName);



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
        static bool FindAbsolutePath(const char* relativePath, GTLib::String &absolutePath);

        /// Retrieves the model definition of the given file name.
        ///
        /// @param fileName [in] The name of the file whose definition is being retrieved.
        ///
        /// @return A pointer to the definition, or null if does not exist.
        static ModelDefinition* FindDefinition(const char* fileName);
    };
}

#endif