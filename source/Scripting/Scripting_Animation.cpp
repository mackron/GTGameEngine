// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting/Scripting_Animation.hpp>

namespace GTEngine
{
    namespace Scripting
    {
        bool LoadAnimationLibrary(GTCore::Script &script)
        {
            script.Execute
            (
                "GTEngine.__AnimationSequence = {};"
                "GTEngine.__AnimationSequence.__index = GTEngine.__AnimationSequence;"
                "GTEngine.__AnimationSequence.isAnimationSequence  = true;"

                "function GTEngine.AnimationSequence(name)"
                "    local new = {};"
                "    setmetatable(new, GTEngine.__AnimationSequence);"
                "        new.Name   = name;"
                "        new.Frames = {};"
                "    return new;"
                "end;"

                "function GTEngine.__AnimationSequence:AddFrame(segmentName, transitionTime, loop)"
                "    local frame = {};"
                "    frame.SegmentName    = segmentName;"
                "    frame.TransitionTime = transitionTime;"
                "    frame.Loop           = loop;"
                ""
                "    self.Frames[#self.Frames + 1] = frame;"
                ""
                "    return frame;"
                "end;"
            );

            return true;
        }


        AnimationSequence ToAnimationSequence(GTCore::Script &script, int position)
        {
            AnimationSequence result;

            if (script.IsTable(position))
            {
                script.Push("Name");
                script.GetTableValue((position > 0) ? position : position - 1);
                if (script.IsString(-1))
                {
                    result.SetName(script.ToString(-1));
                }
                script.Pop(1);


                script.Push("Frames");
                script.GetTableValue((position > 0) ? position : position - 1);
                if (script.IsTable(-1))
                {
                    for (script.PushNil(); script.Next(-2); script.Pop(1))
                    {
                        GTCore::String segmentName;
                        double         transitionTime = 0.0;
                        bool           loop = false;

                        script.Push("SegmentName");
                        script.GetTableValue(-2);
                        if (script.IsString(-1))
                        {
                            segmentName = script.ToString(-1);
                        }
                        script.Pop(1);

                        script.Push("TransitionTime");
                        script.GetTableValue(-2);
                        if (script.IsNumber(-1))
                        {
                            transitionTime = script.ToDouble(-1);
                        }
                        script.Pop(1);

                        script.Push("Loop");
                        script.GetTableValue(-2);
                        if (script.IsBoolean(-1))
                        {
                            loop = script.ToBoolean(-1);
                        }
                        script.Pop(1);


                        result.AddFrame(segmentName.c_str(), transitionTime, loop);
                    }
                }
                script.Pop(1);
            }

            return result;
        }
    }
}
