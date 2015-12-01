// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIStyleScriptCompiler
#define GT_GUIStyleScriptCompiler

#include <GTEngine/Core/String.hpp>
#include <GTEngine/Core/Vector.hpp>
#include <GTEngine/Core/Dictionary.hpp>
#include "GUIStyleScriptError.hpp"

namespace GT
{
    /// Class representing an attribute inside a style class. This is just a simple key/value string pair.
    class GUIStyleScriptCompilerClassAttribute
    {
    public:

        /// Constructor.
        GUIStyleScriptCompilerClassAttribute();

        /// Copy constructor.
        GUIStyleScriptCompilerClassAttribute(const GUIStyleScriptCompilerClassAttribute &other);

        
        /// Sets the name of the attribute.
        void SetName(const char* name, ptrdiff_t sizeInTs = -1);

        /// Retrieves the name of the attribute.
        const char* GetName() const;


        /// Sets the value of the attribute.
        void SetValue(const char* value, ptrdiff_t sizeInTs = -1);

        /// Retrieves the name of the attribute.
        const char* GetValue() const;


        /// Assignment operator.
        GUIStyleScriptCompilerClassAttribute & operator=(const GUIStyleScriptCompilerClassAttribute &other);


    private:

        /// The key/name of the attribute.
        GT::String m_name;

        /// The value of the attribute.
        GT::String m_value;
    };


    /// Class representing a style class for use by the compiler.
    class GUIStyleScriptCompilerClass
    {
    public:

        /// Constructor.
        GUIStyleScriptCompilerClass();

        /// Copy constructor.
        GUIStyleScriptCompilerClass(const GUIStyleScriptCompilerClass &other);

        /// Destructor.
        ~GUIStyleScriptCompilerClass();


        /// Sets the name of the class.
        void SetName(const char* name, ptrdiff_t sizeInTs = -1);

        /// Retrieves the name of the class.
        const char* GetName() const;


        /// Sets the includes string.
        void SetIncludes(const char* includes, ptrdiff_t sizeInTs = -1);

        /// Retrieves the includes string.
        const char* GetIncludes() const;


        /// Retrieves a reference to the internal list of attributes.
              GT::Vector<GUIStyleScriptCompilerClassAttribute> & GetAttributes()       { return this->m_attributes; }
        const GT::Vector<GUIStyleScriptCompilerClassAttribute> & GetAttributes() const { return this->m_attributes; }

        /// Adds a new attribute.
        ///
        /// @param attribute [in] The attribute to add.
        ///
        /// @remarks
        ///     If an attribute of the same name already exists, it will be replaced.
        ///     @par
        ///     This creates a copy of the input attribute. You can delete the input attribute after returning from this call.
        void AddAttribute(const GUIStyleScriptCompilerClassAttribute &attribute);

        /// Removes an attribute by it's index.
        ///
        /// @param index [in] The index of the attribute to remove.
        void RemoveAttributeByIndex(size_t index);

        /// Finds the index of an attribute by it's name, returning whether or not it was found.
        ///
        /// @param name  [in ] The name of the attribute to find.
        /// @param index [out] A reference to the variable that will receive the index if it is found. This will be left undefind if it is not found.
        ///
        /// @return True if the attribute exists; false otherwise.
        bool FindAttributeByName(const char* name, size_t &index) const;

        /// Retrieves the number of attributes in the class.
        size_t GetAttributeCount() const;

        /// Retrieves an attribute by it's index.
        ///
        /// @param index [in] The index of the attribute to retrieve.
              GUIStyleScriptCompilerClassAttribute & GetAttributeByIndex(size_t index);
        const GUIStyleScriptCompilerClassAttribute & GetAttributeByIndex(size_t index) const;


        /// Retrieves a reference to the internal list of sub-classes.
              GT::Dictionary<GUIStyleScriptCompilerClass> & GetSubClasses()       { return this->m_subclasses; }
        const GT::Dictionary<GUIStyleScriptCompilerClass> & GetSubClasses() const { return this->m_subclasses; }

        /// Retrieves a pointer to the sub-class of the given type.
        ///
        /// @param type [in] A string representing the type of the sub-class.
        ///
        /// @remarks
        ///     The returned pointer will become invalid when a sub-class is added or removed.
              GUIStyleScriptCompilerClass* GetSubClass(const char* type, ptrdiff_t sizeInTs = -1);
        const GUIStyleScriptCompilerClass* GetSubClass(const char* type, ptrdiff_t sizeInTs = -1) const;


        /// Adds a new sub-class.
        ///
        /// @param modifierName         [in] The name of the modifier.
        /// @param modifierNameSizeInTs [in] The size of the modifier name. Can be -1 if it's null terminated.
        /// @param subClass             [in] A reference to the sub-class.
        ///
        /// @remarks
        ///     If a sub-class of the given modifier already exists, it new class will be merged. Otherwise a copy will be added.
        void AddSubClass(const char* modifierName, ptrdiff_t modifierNameSizeInTs, const GUIStyleScriptCompilerClass &subClass);
        void AddSubClass(const char* modifierName, const GUIStyleScriptCompilerClass &subClass) { this->AddSubClass(modifierName, -1, subClass); }


        /// Merges the given class into this one.
        ///
        /// @param other [in] A reference to the other class to merge with this one.
        ///
        /// @param
        ///     Merging will replace existing attributes with those defined in 'other'. Includes will be appended to the end.
        ///     @par
        ///     Sub-classes are ignored. They should be merged and handled at a higher level.
        ///     @par
        ///     The name is left unmodified.
        void Merge(const GUIStyleScriptCompilerClass &other);


        /// Assignment operator.
        GUIStyleScriptCompilerClass & operator=(const GUIStyleScriptCompilerClass &other);


    private:

        /// The name of the class.
        GT::String m_name;

        /// A space-delimited list of includes.
        GT::String m_includes;

        /// The list of attributes. We use a vector instead of a map here because we want to keep the order that the
        /// attributes are defined.
        GT::Vector<GUIStyleScriptCompilerClassAttribute> m_attributes;

        /// The list of sub-classes, keyed by the name of the modifier the sub-class.
        GT::Dictionary<GUIStyleScriptCompilerClass> m_subclasses;
    };


    /// Class representing a variable for use by the compiler. This is just a simple key/value string pair.
    class GUIStyleScriptCompilerVariable
    {
    public:

        /// Constructor.
        GUIStyleScriptCompilerVariable();

        /// Copy constructor.
        GUIStyleScriptCompilerVariable(const GUIStyleScriptCompilerVariable &other);

        
        /// Sets the name of the attribute.
        void SetName(const char* name, ptrdiff_t sizeInTs = -1);

        /// Retrieves the name of the attribute.
        const char* GetName() const;


        /// Sets the value of the attribute.
        void SetValue(const char* value, ptrdiff_t sizeInTs = -1);

        /// Retrieves the name of the attribute.
        const char* GetValue() const;


        /// Assignment operator.
        GUIStyleScriptCompilerVariable & operator=(const GUIStyleScriptCompilerVariable &other);


    private:

        /// The key/name of the attribute.
        GT::String m_name;

        /// The value of the attribute.
        GT::String m_value;
    };


    /// Base class for handling compilation errors.
    class GUIStyleScriptCompilerErrorHandler
    {
    public:

        /// Constructor.
        GUIStyleScriptCompilerErrorHandler()
        {
        }

        /// Destructor.
        virtual ~GUIStyleScriptCompilerErrorHandler()
        {
        }


        /// Called when an error occurs.
        virtual void OnError(const GUIStyleScriptError &error) = 0;
    };


    /// Class for compiling a style script.
    class GUIStyleScriptCompiler
    {
    public:

        /// Constructor.
        GUIStyleScriptCompiler();

        /// Constructor.
        GUIStyleScriptCompiler(const char* identifier);
        
        /// Copy constructor.
        GUIStyleScriptCompiler(const GUIStyleScriptCompiler &other);

        /// Destructor.
        ~GUIStyleScriptCompiler();


        /// Compiles the given script.
        ///
        /// @param script      [in] The script to compile.
        /// @param baseURLPath [in] The base URL to use when loading 'url' values. If this is null, it will use the current directory.
        bool Compile(const char* script, const char* baseURLPath = nullptr);


        /// Retrieves the identifier string.
        const char* GetIdentifier() const;


        /// Retrieves the variable count.
        size_t GetClassCount() const;

        /// Retrieves a variable by it's index.
              GUIStyleScriptCompilerClass & GetClassByIndex(size_t index);
        const GUIStyleScriptCompilerClass & GetClassByIndex(size_t index) const;

        /// Retrieves a pointer to a class by it's name.
        ///
        /// @param name [in] The class name.
        GUIStyleScriptCompilerClass* FindClassByName(const char* name, ptrdiff_t nameLengthInTs = -1);

        

        /// Retrieves the variable count.
        size_t GetVariableCount() const;

        /// Retrieves a variable by it's index.
              GUIStyleScriptCompilerVariable & GetVariableByIndex(size_t index);
        const GUIStyleScriptCompilerVariable & GetVariableByIndex(size_t index) const;

        /// Retrieves a pointer to a variable by it's name.
        ///
        /// @param name           [in] The variable name.
        /// @param nameLengthInTs [in] The length of the name of the variable.
        GUIStyleScriptCompilerVariable* FindVariableByName(const char* name, ptrdiff_t nameLengthInTs = -1);



        /// Retrieves a pointer to the given class attribute, if any.
        ///
        /// @param className         [in] The name of the class.
        /// @param classModifierName [in] The name of the sub-class modifier ("hovered", etc).
        /// @param attributeName     [in] The name of the attribute.
              GUIStyleScriptCompilerClassAttribute* GetClassAttribute(const char* className, const char* classModifierName, const char* attributeName);
        const GUIStyleScriptCompilerClassAttribute* GetClassAttribute(const char* className, const char* classModifierName, const char* attributeName) const;

        
        /// Sets the error handler.
        ///
        /// @param errorHandler [in] A reference to the new error handler.
        ///
        /// @remarks
        ///     The new error handler can be null.
        void SetErrorHandler(GUIStyleScriptCompilerErrorHandler* errorHandler);
        void SetErrorHandler(GUIStyleScriptCompilerErrorHandler &errorHandler) { this->SetErrorHandler(&errorHandler); }

        /// Retrieves a pointer to the error handler.
        GUIStyleScriptCompilerErrorHandler* GetErrorHandler();



        /// Assignment operator.
        GUIStyleScriptCompiler & operator=(const GUIStyleScriptCompiler &other);


    private:

        /// Posts an error to the attached event handler.
        ///
        /// @param error [in] A reference to the structure containing the error information.
        void PostError(const GUIStyleScriptError &error);


    private:

        /// The identifier, if any. This will be an empty string if no identifier is specified.
        GT::String m_identifier;

        /// The list of classes.
        GT::Vector<GUIStyleScriptCompilerClass*> m_classes;
        
        /// The list of variables.
        GT::Vector<GUIStyleScriptCompilerVariable*> m_variables;

        /// A pointer to the error handler. Can be null.
        GUIStyleScriptCompilerErrorHandler* m_errorHandler;
        
        
        
    };
}

#endif
