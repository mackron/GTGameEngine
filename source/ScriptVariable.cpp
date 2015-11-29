// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/ScriptVariable.hpp>

namespace GT
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
        : ScriptVariable(ScriptVariableType_Number, name), value(0.0)
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
        : ScriptVariable(ScriptVariableType_Vec4, name), x(0.0), y(0.0), z(0.0), w(0.0)
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
                return new ScriptVariable_Vec3(static_cast<const ScriptVariable_Vec3 &>(variableToCopy));
            }

        case ScriptVariableType_Vec4:
            {
                return new ScriptVariable_Vec4(static_cast<const ScriptVariable_Vec4 &>(variableToCopy));
            }

        case ScriptVariableType_Boolean:
            {
                return new ScriptVariable_Boolean(static_cast<const ScriptVariable_Boolean &>(variableToCopy));
            }

        case ScriptVariableType_String:
            {
                return new ScriptVariable_String(static_cast<const ScriptVariable_String &>(variableToCopy));
            }
        
        case ScriptVariableType_Prefab:
            {
                return new ScriptVariable_Prefab(static_cast<const ScriptVariable_Prefab &>(variableToCopy));
            }

        case ScriptVariableType_Unknown:
        default: break;
        }


        return nullptr;
    }
}
