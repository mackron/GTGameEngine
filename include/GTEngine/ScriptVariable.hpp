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
}

#endif