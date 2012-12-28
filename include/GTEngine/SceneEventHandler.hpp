
#ifndef __GTEngine_SceneEventHandler_hpp_
#define __GTEngine_SceneEventHandler_hpp_

namespace GTEngine
{
    class Scene;
    class SceneObject;

    /// Class for intercepting events from a scene.
    ///
    /// This is really useful for special cases where you might need to do things in response to such things as adding or removing scene nodes. A prime
    /// example of this is editors, where you might want to add metadata to an object as it is added to the editor. The engine's official scene editor
    /// does exactly this.
    class SceneEventHandler
    {
    public:

        /// Constructor.
        SceneEventHandler();

        /// Destructor.
        ~SceneEventHandler();


        /// Called when a scene object is added to the scene.
        ///
        /// @param object [in] A reference to the object that was just added.
        virtual void OnObjectAdded(SceneObject &object);

        /// Called when a scene object is being removed from the scene.
        ///
        /// @param object [in] A reference to the object that is being removed.
        virtual void OnObjectRemoved(SceneObject &object);

        /// Called just after a scene object has been refreshed.
        ///
        /// @param object [in] A reference to the object that has just been refreshed.
        virtual void OnObjectRefreshed(SceneObject &object);


    private:



    };
}

#endif