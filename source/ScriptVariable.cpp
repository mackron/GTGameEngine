// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ScriptVariable.hpp>

namespace GTEngine
{
    ScriptVariable::ScriptVariable(ScriptVariableType typeIn, const char* nameIn)
        : type(typeIn), name(nameIn)
    {
    }

    ScriptVariable::ScriptVariable(const ScriptVariable &other)
        : type(other.GetType()), name(other.GetName())
    {
    }


    ScriptVariable::~ScriptVariable()
    {
    }


    ScriptVariableType ScriptVariable::GetType() const
    {
        return this->type;
    }

    const char* ScriptVariable::GetName() const
    {
        return this->name.c_str();
    }



    /////////////////////////////////////
    // Number

    ScriptVariable_Number::ScriptVariable_Number(const char* name)
        : ScriptVariable(ScriptVariableType_Number, name)
    {
    }

    ScriptVariable_Number::ScriptVariable_Number(const ScriptVariable_Number &other)
        : ScriptVariable(other), value(other.GetValue())
    {
    }

    ScriptVariable_Number::~ScriptVariable_Number()
    {
    }


    /////////////////////////////////////
    // Vec2

    ScriptVariable_Vec2::ScriptVariable_Vec2(const char* name)
        : ScriptVariable(ScriptVariableType_Vec2, name), x(0.0), y(0.0)
    {
    }

    ScriptVariable_Vec2::ScriptVariable_Vec2(const ScriptVariable_Vec2 &other)
        : ScriptVariable(other), x(other.GetX()), y(other.GetY())
    {
    }

    ScriptVariable_Vec2::~ScriptVariable_Vec2()
    {
    }


    /////////////////////////////////////
    // Vec3

    ScriptVariable_Vec3::ScriptVariable_Vec3(const char* name)
        : ScriptVariable(ScriptVariableType_Vec3, name), x(0.0), y(0.0), z(0.0)
    {
    }

    ScriptVariable_Vec3::ScriptVariable_Vec3(const ScriptVariable_Vec3 &other)
        : ScriptVariable(other), x(other.GetX()), y(other.GetY()), z(other.GetZ())
    {
    }

    ScriptVariable_Vec3::~ScriptVariable_Vec3()
    {
    }


    /////////////////////////////////////
    // Vec4

    ScriptVariable_Vec4::ScriptVariable_Vec4(const char* name)
        : ScriptVariable(ScriptVariableType_Vec4, name), x(0.0), y(0.0), z(0.0)
    {
    }

    ScriptVariable_Vec4::ScriptVariable_Vec4(const ScriptVariable_Vec4 &other)
        : ScriptVariable(other), x(other.GetX()), y(other.GetY()), z(other.GetZ()), w(other.GetW())
    {
    }

    ScriptVariable_Vec4::~ScriptVariable_Vec4()
    {
    }


    /////////////////////////////////////
    // Boolean

    ScriptVariable_Boolean::ScriptVariable_Boolean(const char* name)
        : ScriptVariable(ScriptVariableType_Boolean, name), value(false)
    {
    }

    ScriptVariable_Boolean::ScriptVariable_Boolean(const ScriptVariable_Boolean &other)
        : ScriptVariable(other), value(other.GetValue())
    {
    }

    ScriptVariable_Boolean::~ScriptVariable_Boolean()
    {
    }


    /////////////////////////////////////
    // String

    ScriptVariable_String::ScriptVariable_String(const char* name)
        : ScriptVariable(ScriptVariableType_String, name), value()
    {
    }

    ScriptVariable_String::ScriptVariable_String(const ScriptVariable_String &other)
        : ScriptVariable(other), value(other.GetValue())
    {
    }

    ScriptVariable_String::~ScriptVariable_String()
    {
    }


    /////////////////////////////////////
    // Prefab

    ScriptVariable_Prefab::ScriptVariable_Prefab(const char* name)
        : ScriptVariable(ScriptVariableType_Prefab, name), value()
    {
    }

    ScriptVariable_Prefab::ScriptVariable_Prefab(const ScriptVariable_Prefab &other)
        : ScriptVariable(other), value(other.GetValue())
    {
    }

    ScriptVariable_Prefab::~ScriptVariable_Prefab()
    {
    }






    /////////////////////////////////////
    // Statics

    ScriptVariable* ScriptVariable::CreateCopy(const ScriptVariable &variableToCopy)
    {
        switch (variableToCopy.GetType())
        {
        case ScriptVariableType_Number:
            {
                return new ScriptVariable_Number(static_cast<const ScriptVariable_Number &>(variableToCopy));
            }

        case ScriptVariableType_Vec2:
            {
                return new ScriptVariable_Vec2(static_cast<const ScriptVariable_Vec2 &>(variableToCopy));
            }

        case ScriptVariableType_Vec3:
            {
                return nullptr;
            }

        case ScriptVariableType_Vec4:
            {
                return nullptr;
            }

        case ScriptVariableType_String:
            {
                return nullptr;
            }
        
        case ScriptVariableType_Prefab:
            {
                return nullptr;
            }


        default: break;
        }


        return nullptr;
    }
}