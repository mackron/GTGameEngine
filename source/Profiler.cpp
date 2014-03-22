// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Profiler.hpp>
#include <GTLib/Math.hpp>

namespace GTEngine
{
    Profiler::Profiler()
        : maxAccumulatedFrameTime(1.0),
          frameBenchmarker(),    updateBenchmarker(),    renderingBenchmarker(),
          averageFrameTime(0.0), averageUpdateTime(0.0), averageRenderingTime(0.0),
          isEnabled(false)
    {
    }

    Profiler::~Profiler()
    {
    }

    void Profiler::Enable()
    {
        this->isEnabled = true;
    }

    void Profiler::Disable()
    {
        this->isEnabled = false;
    }

    bool Profiler::IsEnabled() const
    {
        return this->isEnabled;
    }


    double Profiler::GetAverageFrameTime() const
    {
        return this->averageFrameTime;
    }

    double Profiler::GetAverageUpdateTime() const
    {
        return this->averageUpdateTime;
    }

    double Profiler::GetAverageRenderingTime() const
    {
        return this->averageRenderingTime;
    }


    double Profiler::GetMaxAccumulatedFrameTime() const
    {
        return this->maxAccumulatedFrameTime;
    }

    void Profiler::SetMaxAccumulatedFrameTime(double newMax)
    {
        this->maxAccumulatedFrameTime = newMax;
    }


    void Profiler::OnBeginFrame()
    {
        // We need to check if each of the benchmarkers need to be reset.
        if (this->frameBenchmarker.GetTotalTime() > this->maxAccumulatedFrameTime)
        {
            this->CalculateAverages();
            this->Reset();
        }

        this->frameBenchmarker.Start();
    }

    void Profiler::OnEndFrame()
    {
        this->frameBenchmarker.End();
    }


    void Profiler::OnBeginUpdate()
    {
        this->updateBenchmarker.Start();
    }

    void Profiler::OnEndUpdate()
    {
        this->updateBenchmarker.End();
    }


    void Profiler::OnBeginRendering()
    {
        this->renderingBenchmarker.Start();
    }

    void Profiler::OnEndRendering()
    {
        this->renderingBenchmarker.End();
    }



    ///////////////////////////////////////////
    // Private

    void Profiler::CalculateAverages()
    {
        this->averageFrameTime     = this->frameBenchmarker.GetAverageTime();
        this->averageUpdateTime    = this->updateBenchmarker.GetAverageTime();
        this->averageRenderingTime = this->renderingBenchmarker.GetAverageTime();
    }

    void Profiler::Reset()
    {
        this->frameBenchmarker.Reset();
        this->updateBenchmarker.Reset();
        this->renderingBenchmarker.Reset();
    }
}
