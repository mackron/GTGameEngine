// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_EngineContext_hpp_
#define __GT_GE_EngineContext_hpp_

#include <GTLib/ResultCodes.hpp>

namespace GT
{
    namespace GE
    {
        /// Class representing the engine context.
        ///
        /// Only a single engine context is usually needed for an application, however it is possible to have more.
        class EngineContext
        {
        public:

            /// Constructor.
            EngineContext();

            /// Destructor.
            ~EngineContext();


            /// Starts up the engine context.
            ///
            /// @param argc [in] The number of arguments passed to the command line (same as the argc parameter of main())
            /// @param argv [in] The array of strings making up the command line (same as the argv parameter of main())
            ///
            /// @return A result code specifying whether or not the context was initialized successfully. If the return value is >= 0, it was successful, otherwise it failed.
            ///
            /// @remarks
            ///     The argc and argv parameters should be passed exactly as they were passed from main().
            ResultCode Startup(int argc, char** argv);

            /// Shuts down the engine engine.
            ///
            /// @remarks
            ///     This does not delete the context object.
            void Shutdown();


        private:





        private:    // No copying.
            EngineContext(const EngineContext &);
            EngineContext & operator=(const EngineContext &);
        };
    }
}


#endif
