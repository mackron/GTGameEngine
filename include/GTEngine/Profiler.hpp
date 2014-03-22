// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Profiler_hpp_
#define __GTEngine_Profiler_hpp_

#include <GTLib/Timing.hpp>

namespace GTEngine
{
    /// Class doing profiling. There is one of these for each game.
    ///
    /// This does not have a GUI. Instead, it just stores data which can then be plugged into a GUI.
    ///
    /// This class works by having the game post events to the profiler.
    class Profiler
    {
    public:

        /// Constructor.
        Profiler();

        /// Destructor.
        ~Profiler();


        /// Enables the profiler.
        ///
        /// @remarks
        ///     This doesn't really do too much except for mark the profiler as enabled.
        void Enable();

        /// Disables the profiler.
        ///
        /// @remarks
        ///     Like Enable(), this mainly just marks the profiler as disabled.
        void Disable();

        /// Determines whether or not the profiler is enabled.
        ///
        /// @return True if the profiler is enabled.
        bool IsEnabled() const;



        /// Retrieves the average frame time.
        double GetAverageFrameTime() const;

        /// Retrieves the average update time.
        double GetAverageUpdateTime() const;

        /// Retrieves the average rendering time.
        double GetAverageRenderingTime() const;



        /// Retrieves the accumulated frame time.
        ///
        /// @remarks
        ///     The accumulated frame time is reset after 
        double GetAccumulatedFrameTime() const;

        /// Retrieves the maximum accumulated frame time before it is reset.
        double GetMaxAccumulatedFrameTime() const;

        /// Sets the maximum accumulated frame time before it is reset.
        void SetMaxAccumulatedFrameTime(double newMax);



        ////////////////////////////////////
        // Events

        /// Called when the next frame begins.
        ///
        /// @remarks
        ///     If the accumulated frame time is larger than GetMaxAccumulatedFrameTime(), which is one second by default.
        void OnBeginFrame();

        /// Called when the next frame ends.
        void OnEndFrame();


        /// Called when the update thread has started.
        void OnBeginUpdate();

        /// Called when the update thread ends.
        void OnEndUpdate();


        /// Called when the rendering thread has started.
        void OnBeginRendering();

        /// Called when the rendering thread ends.
        void OnEndRendering();



    private:

        /// Calculates the averages of each timer.
        void CalculateAverages();

        /// Resets everything.
        void Reset();



    private:

        /// The maximum accumulated frame time before it is reset.
        double maxAccumulatedFrameTime;


        /// The benchmarker for the whole frame.
        GTLib::Benchmarker frameBenchmarker;

        /// The benchmarker for the update thread.
        GTLib::Benchmarker updateBenchmarker;

        /// the benchmarker for the rendering thread.
        GTLib::Benchmarker renderingBenchmarker;



        /// The average frame time.
        double averageFrameTime;

        /// The average update time.
        double averageUpdateTime;

        /// The average rendering time.
        double averageRenderingTime;




        /// Keeps track of whether or not the profiler is enabled.
        bool isEnabled;

    };
}

#endif