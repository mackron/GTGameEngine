
#ifndef __GTEngine_RenderCommands_hpp_
#define __GTEngine_RenderCommands_hpp_

#include "VertexArray.hpp"
#include "DrawModes.hpp"

namespace GTEngine
{
    /**
    *   \brief  Base class for render commands.
    *
    *   
    */
    class RenderCommand
    {
    public:

        /**
        *   \brief  Constructor.
        */
        RenderCommand();

        /**
        *   \brief  Destructor.
        *
        *   \remarks
        *       The destructor will remove the command from the chain, but will not keep the chain connected. If a
        *       command is destructed while still part of a chain, that chain will need to be reconnected manually
        *       if required.
        */
        virtual ~RenderCommand();


        /**
        *   \brief               Sets the rendering command that will come after this one.
        *   \param  command [in] A pointer to the command that will come after this one. Can be null, see remarks.
        *
        *   \remarks
        *       If \c command is null, it will detach all commands that come after this one, making this command the
        *       last one in the chain.
        */
        void SetNext(RenderCommand *command);

        /**
        *   \brief  Sets the rendering command that will come before this one.
        *   \param  command [in] A pointer to the command that will come before this one. Can be null, see remarks.
        *
        *   \remarks
        *       If \c command is null, it will detach all commands that come before this one, making this command the
        *       first one in the chain.
        */
        void SetPrevious(RenderCommand *command);

        /**
        *   \brief  Removes the next command from the chain, and then uses the following command as the new next command.
        *
        *   \remarks
        *       This won't neccessarilly make this command the last in the chain. To remove ALL next commands, use SetNext(nullptr).
        */
        void RemoveNext();

        /**
        *   \brief  Removes the previous command from the chain, and the uses the previous command after that as the new previous.
        *
        *   \remarks
        *       This won't neccessarilly make this command the first in the chain. To remove ALL previous commands in the chain
        *       use SetPrevious(nullptr).
        */
        void RemovePrevious();



        // TODO: Remove OnExecuted(). We shouldn't need this. Applications should be able to manager render commands.
        virtual void OnExecuted() {}



    public:

        /**
        *   \brief  Virtual function that will perform the actual rendering.
        */
        virtual void Execute() = 0;

        


    public: 

        /// A pointer to the draw call that will be executed after this one. This can be nullptr, in which case nothing comes
        /// after this call.
        RenderCommand *next;

        /// A pointer to the render command that will be executed before this one. This can be nullptr, in which case nothing comes
        /// after this call.
        RenderCommand *prev;


    private:
        RenderCommand(const RenderCommand &);
        RenderCommand & operator=(const RenderCommand &);
    };
}


// ExecuteRCBuffer
namespace GTEngine
{
    class RenderCommandBuffer;

    /**
    *   \brief  A special rendering command that will execute a while render command buffer.
    */
    class RenderCommand_ExecuteRCBuffer : public RenderCommand
    {
    public:

        /// Constructor.
        RenderCommand_ExecuteRCBuffer()
            : rcBuffer(nullptr)
        {
        }

        /// Execute.
        void Execute();


    public:

        /// The RC buffer to execute.
        RenderCommandBuffer *rcBuffer;


    private:    // No copying.
        RenderCommand_ExecuteRCBuffer(const RenderCommand_ExecuteRCBuffer &);
        RenderCommand_ExecuteRCBuffer & operator=(const RenderCommand_ExecuteRCBuffer &);
    };
}

#endif