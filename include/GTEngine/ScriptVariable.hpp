// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ScriptVariable_hpp_
#define __GTEngine_ScriptVariable_hpp_

#include "ScriptVariableTypes.hpp"
#include <GTCore/String.hpp>

namespace GTEngine
{
    /// Class representing a public variable in a scene node script.
    ///
    /// The main purpose to this class is to make it possible to build an interface for editting tools.
    class ScriptVariable
    {
    public:

        /// Constructor.
        ScriptVariable(ScriptVariableType type, const char* name);

        /// Copy constructor.
        ScriptVariable(const ScriptVariable &other);


        /// Destructor.
        virtual ~ScriptVariable();


        /// Retrieves the type of the variable.
        ScriptVariableType GetType() const;

        /// Retrieves the name of the variable.
        const char* GetName() const;



        
        ////////////////////////////////////////////////////
        // Statics

        /// Static function for creating a copy of the given variable.
        ///
        /// @param variableToCopy [in] A reference to the variable to copy.
        ///
        /// @remarks
        ///     This is allocated with 'new'. Delete with 'delete'.
        static ScriptVariable* CreateCopy(const ScriptVariable &variable);




    private:

        /// The type of this variable (number, string, etc).
        ScriptVariableType type;

        /// The name of the variable.
        GTCore::String name;
    };


    /// Number script variable.
    class ScriptVariable_Number : public ScriptVariable
    {
    public:

        /// Constructor.
        ScriptVariable_Number(const char* name);

        /// Copy constructor.
        ScriptVariable_Number(const ScriptVariable_Number &other);


        /// Destructor.
        ~ScriptVariable_Number();


        /// Retrieves the value of the variable.
        double GetValue() const { return this->value; }

        /// Sets the value of the variable.
        void SetValue(double newValue) { this->value = newValue; }



    private:

        /// The value.
        double value;
    };


    /// Vec2 script variable.
    class ScriptVariable_Vec2 : public ScriptVariable
    {
    public:

        /// Constructor.
        ScriptVariable_Vec2(const char* name);

        /// Copy constructor.
        ScriptVariable_Vec2(const ScriptVariable_Vec2 &other);


        /// Destructor.
        ~ScriptVariable_Vec2();


        /// Retrieves the X value.
        double GetX() const { return this->x; }

        /// Retrieves the Y value.
        double GetY() const { return this->y; }


        /// Sets the X value.
        void SetX(double newX) { this->x = newX; }

        /// Sets the Y value.
        void SetY(double newY) { this->y = newY; }

        /// Sets the value of the variable.
        void SetValue(double newX, double newY)
        {
            this->SetX(newX);
            this->SetY(newY);
        }



    private:

        /// The x value.
        double x;

        /// The y value.
        double y;
    };


    /// Vec3 script variable.
    class ScriptVariable_Vec3 : public ScriptVariable
    {
    public:

        /// Constructor.
        ScriptVariable_Vec3(const char* name);

        /// Copy constructor.
        ScriptVariable_Vec3(const ScriptVariable_Vec3 &other);


        /// Destructor.
        ~ScriptVariable_Vec3();


        /// Retrieves the X value.
        double GetX() const { return this->x; }

        /// Retrieves the Y value.
        double GetY() const { return this->y; }

        /// Retrieves the Z value.
        double GetZ() const { return this->z; }


        /// Sets the X value.
        void SetX(double newX) { this->x = newX; }

        /// Sets the Y value.
        void SetY(double newY) { this->y = newY; }

        /// Sets the Z value.
        void SetZ(double newZ) { this->z = newZ; }

        /// Sets the value of the variable.
        void SetValue(double newX, double newY, double newZ)
        {
            this->SetX(newX);
            this->SetY(newY);
            this->SetZ(newZ);
        }



    private:

        /// The x value.
        double x;

        /// The y value.
        double y;

        /// The z value.
        double z;
    };



    /// Vec4 script variable.
    class ScriptVariable_Vec4 : public ScriptVariable
    {
    public:

        /// Constructor.
        ScriptVariable_Vec4(const char* name);

        /// Copy constructor.
        ScriptVariable_Vec4(const ScriptVariable_Vec4 &other);


        /// Destructor.
        ~ScriptVariable_Vec4();


        /// Retrieves the X value.
        double GetX() const { return this->x; }

        /// Retrieves the Y value.
        double GetY() const { return this->y; }

        /// Retrieves the Z value.
        double GetZ() const { return this->z; }

        /// Retrieves the W value.
        double GetW() const { return this->w; }


        /// Sets the X value.
        void SetX(double newX) { this->x = newX; }

        /// Sets the Y value.
        void SetY(double newY) { this->y = newY; }

        /// Sets the Z value.
        void SetZ(double newZ) { this->z = newZ; }

        /// Sets the W value.
        void SetW(double newW) { this->w = newW; }

        /// Sets the value of the variable.
        void SetValue(double newX, double newY, double newZ, double newW)
        {
            this->SetX(newX);
            this->SetY(newY);
            this->SetZ(newZ);
            this->SetW(newW);
        }



    private:

        /// The x value.
        double x;

        /// The y value.
        double y;

        /// The z value.
        double z;

        /// The w value.
        double w;
    };


    /// Boolean script variable
    class ScriptVariable_Boolean : public ScriptVariable
    {
    public:

        /// Constructor.
        ScriptVariable_Boolean(const char* name);

        /// Copy constructor.
        ScriptVariable_Boolean(const ScriptVariable_Boolean &other);


        /// Destructor.
        ~ScriptVariable_Boolean();


        /// Retrieves the value of the variable.
        bool GetValue() const { return this->value; }

        /// Sets the value of the variable.
        void SetValue(bool newValue) { this->value = newValue; }



    private:

        /// The value.
        bool value;
    };



    /// String script variable
    class ScriptVariable_String : public ScriptVariable
    {
    public:

        /// Constructor.
        ScriptVariable_String(const char* name);

        /// Copy constructor.
        ScriptVariable_String(const ScriptVariable_String &other);


        /// Destructor.
        ~ScriptVariable_String();


        /// Retrieves the value of the variable.
        const char* GetValue() const { return this->value.c_str(); }

        /// Sets the value of the variable.
        void SetValue(const char* newValue) { this->value = newValue; }



    private:

        /// The value.
        GTCore::String value;
    };


    /// Prefab script variable. This is actually just a string that stores the relative path to the prefab.
    class ScriptVariable_Prefab : public ScriptVariable
    {
    public:

        /// Constructor.
        ScriptVariable_Prefab(const char* name);

        /// Copy constructor.
        ScriptVariable_Prefab(const ScriptVariable_Prefab &other);


        /// Destructor.
        ~ScriptVariable_Prefab();


        /// Retrieves the value of the variable.
        const char* GetValue() const { return this->value.c_str(); }

        /// Sets the value of the variable.
        void SetValue(const char* newValue) { this->value = newValue; }



    private:

        /// The value.
        GTCore::String value;
    };
}

#endif