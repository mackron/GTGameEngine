// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Editor_SceneEditorPrefabLinker
#define GT_Editor_SceneEditorPrefabLinker

#include "../../DefaultPrefabLinker.hpp"

namespace GT
{
    class SceneEditor;

    class SceneEditorPrefabLinker : public DefaultPrefabLinker
    {
    public:

        /// Constructor.
        SceneEditorPrefabLinker(Scene &scene, SceneEditor &sceneEditor);

        /// Destructor.
        ~SceneEditorPrefabLinker();


    private:

        /// PrefabLinker::OnSceneNodeDeserializeStart()
        void OnSceneNodeDeserializeStart(SceneNode &sceneNode);

        /// PrefabLinker::OnSceneNodeDeserializeEnd()
        void OnSceneNodeDeserializeEnd(SceneNode &sceneNode);


    private:

        /// A reference to the scene editor that owns this linker.
        SceneEditor &sceneEditor;

        /// Keeps track of whether or not the scene node currently being deserialized was selected.
        bool wasSelected;


    private:    // No copying.
        SceneEditorPrefabLinker(const SceneEditorPrefabLinker &);
        SceneEditorPrefabLinker & operator=(const SceneEditorPrefabLinker &);
    };
}

#endif