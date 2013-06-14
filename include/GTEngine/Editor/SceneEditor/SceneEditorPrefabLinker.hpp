// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Editor_SceneEditorPrefabLinker_hpp_
#define __GTEngine_Editor_SceneEditorPrefabLinker_hpp_

#include "../../PrefabLinker.hpp"

namespace GTEngine
{
    class SceneEditor;

    class SceneEditorPrefabLinker : public PrefabLinker
    {
    public:

        /// Constructor.
        SceneEditorPrefabLinker(SceneEditor &sceneEditor);

        /// Destructor.
        ~SceneEditorPrefabLinker();


        /// PrefabLinker::CreateSceneNode()
        SceneNode* CreateSceneNode() const;

        /// PrefabLinker::DeleteSceneNode()
        void DeleteSceneNode(SceneNode* sceneNode) const;



    private:

        /// A reference to the scene editor that owns this linker.
        SceneEditor &sceneEditor;


    private:    // No copying.
        SceneEditorPrefabLinker(const SceneEditorPrefabLinker &);
        SceneEditorPrefabLinker & operator=(const SceneEditorPrefabLinker &);
    };
}

#endif