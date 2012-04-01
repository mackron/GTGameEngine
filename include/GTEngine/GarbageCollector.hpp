
#ifndef __GTEngine_GarbageCollector_hpp_
#define __GTEngine_GarbageCollector_hpp_

namespace GTEngine
{
    class Texture2D;
    class Shader;
    class Framebuffer;
    class VertexArray;

    /// Static class representing the engine's garbage collector.
    ///
    /// The main purspose of the garbage collector is to help with synchronization between threads. In many cases,
    /// there will be times when a resource needs to be destroyed, but may be needed by another thread. This is where
    /// the garbage collector comes in real handy.
    ///
    /// The garbage collector is started up and shutdown by GTEngine::Startup() and GTEngine::Shutdown().
    ///
    /// The garbage collector does not generically support every single object type. Instead it supports specific
    /// resources that can be collected individually.
    ///
    /// Resources are marked for collection with MarkForCollection(). Resources are collected/deleted with Collect().
    /// A collection is performed once every frame on the main/rendering thread.
    ///
    /// All resources in the garbage collector have a counter associated with them. This counter determines how many
    /// collections will take place before the object is deleted entirely. This is set to 1 by default which means the
    /// object will be fully deleted on the SECOND call to Collect(). This mechanism is important since it ensures an
    /// object should never be in use by a thread for a given frame. One such example is if a render command using a
    /// framebuffer is pushed onto back RC queue, and then that framebuffer is marked for collection during that very
    /// same frame. In this case, the end-of-frame routines will collect garbage, but the framebuffer will still be
    /// needed by the rendering thread for the next frame. If the counter mechanism was not in place, the framebuffer
    /// will have been collected too early.
    class GarbageCollector
    {
    // Startup/Shutdown.
    public:

        /// Starts up the garbage collector.
        /// @return True if the garbage collector is started successfully; false otherwise.
        static bool Startup();

        /// Shutsdown the garbage collector.
        ///
        /// @remarks
        ///     This will collect any garbage resources still waiting for collection.
        static void Shutdown();

    
    // Marking.
    public:

        /// Marks a texture for collection.
        /// @param texture [in] The texture being marked for collection.
        /// @param counter [in] The counter to associate with the texture.
        static void MarkForCollection(Texture2D &texture, int counter = 1);

        /// Marks a shader for collection.
        /// @param shader  [in] The shader being marked for collection.
        /// @param counter [in] The counter to associate with the shader.
        static void MarkForCollection(Shader &shader, int counter = 1);

        /// Marks a framebuffer for collection.
        /// @param framebuffer [in] The framebuffer being marked for collection.
        /// @param counter     [in] The counter to associate with the shader.
        static void MarkForCollection(Framebuffer &framebuffer, int counter = 1);

        /// Marks a vertex array for collection.
        /// @param va      [in] The vertex array being marked for collection.
        /// @param counter [in] The counter to associate with the shader.
        static void MarkForCollection(VertexArray &va, int counter = 1);

        
    // Collecting.
    public:

        /// Collects garbage textures.
        static void CollectTexture2Ds();

        /// Collects garbage shaders.
        static void CollectShaders();

        /// Collects garbage framebuffers.
        static void CollectFramebuffers();

        /// Collects garbage vertex arrays.
        static void CollectVertexArrays();
    };
}

#endif