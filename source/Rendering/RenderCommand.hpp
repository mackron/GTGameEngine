// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_RenderCommand_hpp_
#define __GTEngine_RenderCommand_hpp_

namespace GTEngine
{
    /// The base class for render commands. Note the simplicity of this class. The only method an inheritted class needs
    /// to implement is Execute(), which will be called when the command is run on the rendering thread.
    class RenderCommand
    {
    public:

        /// Constructor.
        RenderCommand() {}

        /// Destructor.
        virtual ~RenderCommand() {}

        /// Virtual function that will perform the actual rendering.
        ///
        /// @remarks
        ///     This will be called from the rendering thread. It's where rendering calls should be made on main renderer.
        virtual void Execute() = 0;


    private:
        RenderCommand(const RenderCommand &);
        RenderCommand & operator=(const RenderCommand &);
    };
}

#endif