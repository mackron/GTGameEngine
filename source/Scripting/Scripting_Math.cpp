// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting/Scripting_Math.hpp>
#include <GTEngine/Math.hpp>

namespace GTEngine
{
    namespace Scripting
    {
        bool LoadExtendedMathLibrary(GTCore::Script &script)
        {
            //////////////////////////////////////////
            // Basic Functions.
            script.Execute
            (
                "function math.inversesqrt(value)"
                "    return 1.0 / math.sqrt(value);"
                "end;"

                "function math.radians(degrees)"
                "    return degrees * (math.pi / 180.0);"
                "end;"

                "function math.degrees(radians)"
                "    return radians * (180.0 / math.pi);"
                "end;"
            );


            // TODO: In the operator overloads here we need to handle cases like scalers. Also, vector/quat interations.


            //////////////////////////////////////////
            // math.vec2
            script.Execute
            (
                "math.__vec2 = {};"
                "math.__vec2.__index = math.__vec2;"

                "math.__vec2.__add = function(a, b)"
                "    return math.vec2(a.x + b.x, a.y + b.y);"
                "end;"

                "math.__vec2.__sub = function(a, b)"
                "    return math.vec2(a.x - b.x, a.y - b.y);"
                "end;"

                "math.__vec2.__mul = function(a, b)"
                "    return math.vec2(a.x * b.x, a.y * b.y);"
                "end;"

                "math.__vec2.__div = function(a, b)"
                "    return math.vec2(a.x / b.x, a.y / b.y);"
                "end;"

                "math.__vec2.__eq = function(a, b)"
                "    return a.x == b.x and a.y == b.y;"
                "end;"

                "math.__vec2.__unm = function(op)"
                "    return math.vec2(-op.x, -op.y);"
                "end;"

                "math.__vec2.__len = function(op)"
                "    return 2;"
                "end;"


                "math.__vec2.length = function(self)"
                "    return math.vec2_length(self);"
                "end;"

                "math.__vec2.normalize = function(self)"
                "    local invLenth = 1.0 /  math.vec2_length(self);"
                "    self.x = self.x * invLength;"
                "    self.y = self.y * invLength;"
                ""
                "    return self;"
                "end;"

                "math.__vec2.dot = function(self, b)"
                "    return math.vec2_dot(self, b);"
                "end;"

                "math.__vec2.distance = function(self, b)"
                "    return math.vec2_distance(self, b);"
                "end;"

                "math.__vec2.faceforward = function(self, i, ref)"
                "    if math.vec2_dot(ref, i) >= 0.0 then"
                "        self.x = -self.x;"
                "        self.y = -self.y;"
                "    end;"
                ""
                "    return self;"
                "end;"


                "function math.vec2(xIn, yIn)"
                "    local new = {};"
                "    setmetatable(new, math.__vec2);"
                "        new.x = xIn;"
                "        new.y = yIn or xIn;"
                "    return new;"
                "end;"


                "function math.vec2_dot(a, b)"
                "    return a.x * b.x + a.y * b.y;"
                "end;"

                "function math.vec2_length(a)"
                "    return math.sqrt(math.vec2_dot(a, a));"
                "end;"

                "function math.vec2_normalize(a)"
                "    local invLenth = 1.0 /  math.vec2_length(a);"
                "    return math.vec2("
                "        a.x * invLength,"
                "        a.y * invLength"
                "    );"
                "end;"

                "function math.vec2_distance(a, b)"
                "    local x = b.x - a.x;"
                "    local y = b.y - a.y;"
                "    return math.sqrt(x * x + y * y);"
                "end;"

                "function math.vec2_reflect(i, n)"
                "    local d = math.vec2_dot(n, i);"
                "    return math.vec2("
                "        i.x - 2.0 * d * n.x,"
                "        i.y - 2.0 * d * n.y"
                "    );"
                "end;"

                "function math.vec2_refract(i, n, eta)"
                "    local d = math.vec2_dot(n, i);"
                "    local k = 1.0 - eta * eta * (1.0 - d * d);"
                "    if k < 0.0 then"
                "        return math.vec2(0.0, 0.0);"
                "    else"
                "        local sqrtk = math.sqrt(k);"
                "        return math.vec2("
                "            eta * i.x - (eta * d + sqrtk) * n.x,"
                "            eta * i.y - (eta * d + sqrtk) * n.y"
                "        );"
                "    end;"
                "end;"


                "math.vec2_posx = math.vec2( 1.0,  0.0);"
                "math.vec2_posy = math.vec2( 0.0,  1.0);"
                "math.vec2_negx = math.vec2(-1.0,  0.0);"
                "math.vec2_negy = math.vec2( 0.0, -1.0);"
            );


            //////////////////////////////////////////
            // math.vec3
            script.Execute
            (
                "math.__vec3 = {};"
                "math.__vec3.__index = math.__vec3;"

                "math.__vec3.__add = function(a, b)"
                "    return math.vec3(a.x + b.x, a.y + b.y, a.z + b.z);"
                "end;"

                "math.__vec3.__sub = function(a, b)"
                "    return math.vec3(a.x - b.x, a.y - b.y, a.z - b.z);"
                "end;"

                "math.__vec3.__mul = function(a, b)"
                "    return math.vec3(a.x * b.x, a.y * b.y, a.z * b.z);"
                "end;"

                "math.__vec3.__div = function(a, b)"
                "    return math.vec3(a.x / b.x, a.y / b.y, a.z / b.z);"
                "end;"

                "math.__vec3.__eq = function(a, b)"
                "    return a.x == b.x and a.y == b.y and a.z == b.z;"
                "end;"

                "math.__vec3.__unm = function(op)"
                "    return math.vec3(-op.x, -op.y, -op.z);"
                "end;"

                "math.__vec3.__len = function(op)"
                "    return 3;"
                "end;"


                "math.__vec3.length = function(self)"
                "    return math.vec3_length(self);"
                "end;"

                "math.__vec3.normalize = function(self)"
                "    local invLenth = 1.0 /  math.vec3_length(self);"
                "    self.x = self.x * invLength;"
                "    self.y = self.y * invLength;"
                "    self.z = self.z * invLength;"
                ""
                "    return self;"
                "end;"

                "math.__vec3.dot = function(self, b)"
                "    return math.vec3_dot(self, b);"
                "end;"

                "math.__vec3.cross = function(self, b)"
                "    return math.vec3_cross(self, b);"
                "end;"

                "math.__vec3.distance = function(self, b)"
                "    return math.vec3_distance(self, b);"
                "end;"

                "math.__vec3.faceforward = function(self, i, ref)"
                "    if math.vec3_dot(ref, i) >= 0.0 then"
                "        self.x = -self.x;"
                "        self.y = -self.y;"
                "        self.z = -self.z;"
                "    end;"
                ""
                "    return self;"
                "end;"


                "function math.vec3(xIn, yIn, zIn)"
                "    local new = {};"
                "    setmetatable(new, math.__vec3);"
                "        new.x = xIn;"
                "        new.y = yIn or xIn;"
                "        new.z = zIn or xIn;"
                "    return new;"
                "end;"

                "function math.vec3_dot(a, b)"
                "    return a.x * b.x + a.y * b.y + a.z * b.z;"
                "end;"

                "function math.vec3_length(a)"
                "    return math.sqrt(math.vec3_dot(a, a));"
                "end;"

                "function math.vec3_cross(a, b)"
                "    return math.vec3("
                "        a.y * b.z - b.y * a.z,"
			    "        a.z * b.x - b.z * a.x,"
			    "        a.x * b.y - b.x * a.y"
                "    );"
                "end;"

                "function math.vec3_normalize(a)"
                "    local invLenth = 1.0 /  math.vec3_length(a);"
                "    return math.vec3("
                "        a.x * invLength,"
                "        a.y * invLength,"
                "        a.z * invLength "
                "    );"
                "end;"

                "function math.vec3_distance(a, b)"
                "    local x = b.x - a.x;"
                "    local y = b.y - a.y;"
                "    local z = b.z - a.z;"
                "    return math.sqrt(x * x + y * y + z * z);"
                "end;"

                "function math.vec3_reflect(i, n)"
                "    local d = math.vec3_dot(n, i);"
                "    return math.vec3("
                "        i.x - 2.0 * d * n.x,"
                "        i.y - 2.0 * d * n.y,"
                "        i.z - 2.0 * d * n.z"
                "    );"
                "end;"

                "function math.vec3_refract(i, n, eta)"
                "    local d = math.vec3_dot(n, i);"
                "    local k = 1.0 - eta * eta * (1.0 - d * d);"
                "    if k < 0.0 then"
                "        return math.vec3(0.0, 0.0, 0.0);"
                "    else"
                "        local sqrtk = math.sqrt(k);"
                "        return math.vec3("
                "            eta * i.x - (eta * d + sqrtk) * n.x,"
                "            eta * i.y - (eta * d + sqrtk) * n.y,"
                "            eta * i.z - (eta * d + sqrtk) * n.z"
                "        );"
                "    end;"
                "end;"


                "math.vec3_posx = math.vec3( 1.0,  0.0,  0.0);"
                "math.vec3_posy = math.vec3( 0.0,  1.0,  0.0);"
                "math.vec3_posz = math.vec3( 0.0,  0.0,  1.0);"
                "math.vec3_negx = math.vec3(-1.0,  0.0,  0.0);"
                "math.vec3_negy = math.vec3( 0.0, -1.0,  0.0);"
                "math.vec3_negz = math.vec3( 0.0,  0.0, -1.0);"
            );


            //////////////////////////////////////////
            // math.vec4
            script.Execute
            (
                "math.__vec4 = {};"
                "math.__vec4.__index = math.__vec4;"

                "math.__vec4.__add = function(a, b)"
                "    return math.vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);"
                "end;"

                "math.__vec4.__sub = function(a, b)"
                "    return math.vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);"
                "end;"

                "math.__vec4.__mul = function(a, b)"
                "    return math.vec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);"
                "end;"

                "math.__vec4.__div = function(a, b)"
                "    return math.vec4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);"
                "end;"

                "math.__vec4.__eq = function(a, b)"
                "    return a.x == b.x and a.y == b.y and a.z == b.z and a.w == b.w;"
                "end;"

                "math.__vec4.__unm = function(op)"
                "    return math.vec4(-op.x, -op.y, -op.z, -op.w);"
                "end;"

                "math.__vec4.__len = function(op)"
                "    return 4;"
                "end;"


                "math.__vec4.length = function(self)"
                "    return math.vec4_length(self);"
                "end;"

                "math.__vec4.normalize = function(self)"
                "    local invLenth = 1.0 /  math.vec4_length(self);"
                "    self.x = self.x * invLength;"
                "    self.y = self.y * invLength;"
                "    self.z = self.z * invLength;"
                "    self.w = self.w * invLength;"
                ""
                "    return self;"
                "end;"

                "math.__vec4.dot = function(self, b)"
                "    return math.vec4_dot(self, b);"
                "end;"

                "math.__vec4.distance = function(self, b)"
                "    return math.vec4_distance(self, b);"
                "end;"

                "math.__vec4.faceforward = function(self, i, ref)"
                "    if math.vec4_dot(ref, i) >= 0.0 then"
                "        self.x = -self.x;"
                "        self.y = -self.y;"
                "        self.z = -self.z;"
                "        self.w = -self.w;"
                "    end;"
                ""
                "    return self;"
                "end;"


                "function math.vec4(xIn, yIn, zIn, wIn)"
                "    local new = {};"
                "    setmetatable(new, math.__vec4);"
                "        new.x = xIn;"
                "        new.y = yIn or xIn;"
                "        new.z = zIn or xIn;"
                "        new.w = wIn or xIn;"
                "    return new;"
                "end;"


                "function math.vec4_dot(a, b)"
                "    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;"
                "end;"

                "function math.vec4_length(a)"
                "    return math.sqrt(math.vec4_dot(a, a));"
                "end;"

                "function math.vec4_normalize(a)"
                "    local invLenth = 1.0 /  math.vec4_length(a);"
                "    return math.vec4("
                "        a.x * invLength,"
                "        a.y * invLength,"
                "        a.z * invLength,"
                "        a.w * invLength"
                "    );"
                "end;"

                "function math.vec4_distance(a, b)"
                "    local x = b.x - a.x;"
                "    local y = b.y - a.y;"
                "    local z = b.z - a.z;"
                "    local w = b.w - a.w;"
                "    return math.sqrt(x * x + y * y + z * z + w * w);"
                "end;"

                "function math.vec4_reflect(i, n)"
                "    local d = math.vec4_dot(n, i);"
                "    return math.vec4("
                "        i.x - 2.0 * d * n.x,"
                "        i.y - 2.0 * d * n.y,"
                "        i.z - 2.0 * d * n.z,"
                "        i.w - 2.0 * d * n.w"
                "    );"
                "end;"

                "function math.vec4_refract(i, n, eta)"
                "    local d = math.vec4_dot(n, i);"
                "    local k = 1.0 - eta * eta * (1.0 - d * d);"
                "    if k < 0.0 then"
                "        return math.vec4(0.0, 0.0, 0.0, 0.0);"
                "    else"
                "        local sqrtk = math.sqrt(k);"
                "        return math.vec4("
                "            eta * i.x - (eta * d + sqrtk) * n.x,"
                "            eta * i.y - (eta * d + sqrtk) * n.y,"
                "            eta * i.z - (eta * d + sqrtk) * n.z,"
                "            eta * i.w - (eta * d + sqrtk) * n.w"
                "        );"
                "    end;"
                "end;"


                "math.vec4_posx = math.vec4( 1.0,  0.0,  0.0,  0.0);"
                "math.vec4_posy = math.vec4( 0.0,  1.0,  0.0,  0.0);"
                "math.vec4_posz = math.vec4( 0.0,  0.0,  1.0,  0.0);"
                "math.vec4_posw = math.vec4( 0.0,  0.0,  0.0,  1.0);"
                "math.vec4_negx = math.vec4(-1.0,  0.0,  0.0,  0.0);"
                "math.vec4_negy = math.vec4( 0.0, -1.0,  0.0,  0.0);"
                "math.vec4_negz = math.vec4( 0.0,  0.0, -1.0,  0.0);"
                "math.vec4_posw = math.vec4( 0.0,  0.0,  0.0, -1.0);"
            );


            //////////////////////////////////////////
            // math.quat
            script.Execute
            (
                "math.__quat = {};"
                "math.__quat.__index = math.__quat;"

                "math.__quat.__add = function(a, b)"
                "    return math.quat(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);"
                "end;"

                "math.__quat.__sub = function(a, b)"
                "    return math.quat(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);"
                "end;"

                "math.__quat.__mul = function(a, b)"
                "    return math.quat("
                "        b.w * a.w - b.x * a.x - b.y * a.y - b.z * a.z,"
			    "        b.w * a.x + b.x * a.w + b.y * a.z - b.z * a.y,"
			    "        b.w * a.y + b.y * a.w + b.z * a.x - b.x * a.z,"
			    "        b.w * a.z + b.z * a.w + b.x * a.y - b.y * a.x"
                "    );"
                "end;"

                "math.__quat.__eq = function(a, b)"
                "    return a.x == b.x and a.y == b.y and a.z == b.z and a.w == b.w;"
                "end;"

                "math.__quat.__unm = function(op)"
                "    return math.quat(-op.x, -op.y, -op.z, -op.w);"
                "end;"

                "math.__quat.__len = function(op)"
                "    return 4;"
                "end;"


                "function math.quat(xIn, yIn, zIn, wIn)"
                "    local new = {};"
                "    setmetatable(new, math.__quat);"
                "        new.x = xIn;"
                "        new.y = yIn;"
                "        new.z = zIn;"
                "        new.w = wIn;"
                "    return new;"
                "end;"

                "function math.quat_angleaxis(angle, axis)"
                "    angle = radians(angle);"
                ""
                "    local s = math.sin(angle * 0.5);"
                "    return math.quat("
                "        axis.x * s,"
                "        axis.y * s,"
                "        axis.z * s,"
                "        math.cos(angle * 0.5)"
                "    );"
                "end;"
            );




            return true;
        }



        glm::vec2 ToVector2(GTCore::Script &script, int position)
        {
            glm::vec2 result;

            if (script.IsTable(position))
            {
                script.Push("x");
                script.GetTableValue((position > 0) ? position : position - 1);
                if (script.IsNumber(-1))
                {
                    result.x = script.ToFloat(-1);
                }
                script.Pop(1);


                script.Push("y");
                script.GetTableValue((position > 0) ? position : position - 1);
                if (script.IsNumber(-1))
                {
                    result.y = script.ToFloat(-1);
                }
                script.Pop(1);
            }

            return result;
        }


        glm::vec3 ToVector3(GTCore::Script &script, int position)
        {
            glm::vec3 result;

            if (script.IsTable(position))
            {
                script.Push("x");
                script.GetTableValue((position > 0) ? position : position - 1);
                if (script.IsNumber(-1))
                {
                    result.x = script.ToFloat(-1);
                }
                script.Pop(1);


                script.Push("y");
                script.GetTableValue((position > 0) ? position : position - 1);
                if (script.IsNumber(-1))
                {
                    result.y = script.ToFloat(-1);
                }
                script.Pop(1);


                script.Push("z");
                script.GetTableValue((position > 0) ? position : position - 1);
                if (script.IsNumber(-1))
                {
                    result.z = script.ToFloat(-1);
                }
                script.Pop(1);
            }

            return result;
        }


        glm::vec4 ToVector4(GTCore::Script &script, int position)
        {
            glm::vec4 result;

            if (script.IsTable(position))
            {
                script.Push("x");
                script.GetTableValue((position > 0) ? position : position - 1);
                if (script.IsNumber(-1))
                {
                    result.x = script.ToFloat(-1);
                }
                script.Pop(1);


                script.Push("y");
                script.GetTableValue((position > 0) ? position : position - 1);
                if (script.IsNumber(-1))
                {
                    result.y = script.ToFloat(-1);
                }
                script.Pop(1);


                script.Push("z");
                script.GetTableValue((position > 0) ? position : position - 1);
                if (script.IsNumber(-1))
                {
                    result.z = script.ToFloat(-1);
                }
                script.Pop(1);


                script.Push("w");
                script.GetTableValue((position > 0) ? position : position - 1);
                if (script.IsNumber(-1))
                {
                    result.w = script.ToFloat(-1);
                }
                script.Pop(1);
            }

            return result;
        }


        glm::quat ToQuaternion(GTCore::Script &script, int position)
        {
            glm::quat result;

            if (script.IsTable(position))
            {
                script.Push("x");
                script.GetTableValue((position > 0) ? position : position - 1);
                if (script.IsNumber(-1))
                {
                    result.x = script.ToFloat(-1);
                }
                script.Pop(1);


                script.Push("y");
                script.GetTableValue((position > 0) ? position : position - 1);
                if (script.IsNumber(-1))
                {
                    result.y = script.ToFloat(-1);
                }
                script.Pop(1);


                script.Push("z");
                script.GetTableValue((position > 0) ? position : position - 1);
                if (script.IsNumber(-1))
                {
                    result.z = script.ToFloat(-1);
                }
                script.Pop(1);


                script.Push("w");
                script.GetTableValue((position > 0) ? position : position - 1);
                if (script.IsNumber(-1))
                {
                    result.w = script.ToFloat(-1);
                }
                script.Pop(1);
            }

            return result;
        }



        void PushNewVector2(GTCore::Script &script, float x, float y)
        {
            script.GetGlobal("math");
            assert(script.IsTable(-1));
            {
                script.Push("vec2");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.Push(x);
                    script.Push(y);
                    script.Call(2, 1);

                    // Return value needs to be moved behind "math" in the stack which will leave it in the correct position for us.
                    script.InsertIntoStack(-2);
                }
            }
            script.Pop(1);
        }

        void PushNewVector3(GTCore::Script &script, float x, float y, float z)
        {
            script.GetGlobal("math");
            assert(script.IsTable(-1));
            {
                script.Push("vec3");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.Push(x);
                    script.Push(y);
                    script.Push(z);
                    script.Call(3, 1);

                    // Return value needs to be moved behind "math" in the stack which will leave it in the correct position for us.
                    script.InsertIntoStack(-2);
                }
            }
            script.Pop(1);
        }

        void PushNewVector4(GTCore::Script &script, float x, float y, float z, float w)
        {
            script.GetGlobal("math");
            assert(script.IsTable(-1));
            {
                script.Push("vec4");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.Push(x);
                    script.Push(y);
                    script.Push(z);
                    script.Push(w);
                    script.Call(4, 1);

                    // Return value needs to be moved behind "math" in the stack which will leave it in the correct position for us.
                    script.InsertIntoStack(-2);
                }
            }
            script.Pop(1);
        }
    }
}