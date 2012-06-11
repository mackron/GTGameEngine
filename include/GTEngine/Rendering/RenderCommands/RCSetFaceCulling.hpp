
#ifndef __GTEngine_RCSetFaceCulling_hpp_
#define __GTEngine_RCSetFaceCulling_hpp_

#include "../RenderCommand.hpp"

namespace GTEngine
{
    /// Generic command for setting the face culling mode.
    class RCSetFaceCulling : public RenderCommand
    {
    // Methods below are called on the non-rendering thread.
    public:

        /// Constructor.
        RCSetFaceCulling();

        /// Destructor
        ~RCSetFaceCulling();


        /// Sets the culling mode.
        ///
        /// @param cullFrontFaces [in] Whether or not to cull front faces.
        /// @param cullBackFaces  [in] Whether or not to cull back faces.
        void SetCullingMode(bool cullFrontFaces, bool cullBackFaces);


    // The methods below are called on the rendering thread.
    public:

        /// RenderCommand::Execute().
        void Execute();


    private:

        /// Whether or not to cull front faces.
        bool cullFrontFaces;

        /// Whether or not cull back faces.
        bool cullBackFaces;
    };
}

#endif
