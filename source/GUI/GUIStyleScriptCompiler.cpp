// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/GUI/GUIStyleScriptCompiler.hpp>
#include "GUIStyleTokenizer.hpp"
#include <dr_libs/dr_fs.h>
#include <dr_libs/dr_path.h>

namespace GT
{
    ///////////////////////////////////////////////
    // GUIStyleScriptCompilerClassAttribute

    GUIStyleScriptCompilerClassAttribute::GUIStyleScriptCompilerClassAttribute()
        : m_name(),
          m_value()
    {
    }

    GUIStyleScriptCompilerClassAttribute::GUIStyleScriptCompilerClassAttribute(const GUIStyleScriptCompilerClassAttribute &other)
        : m_name(other.GetName()),
          m_value(other.GetValue())
    {
    }


    void GUIStyleScriptCompilerClassAttribute::SetName(const char* name, ptrdiff_t nameSizeInTs)
    {
        this->m_name.Assign(name, nameSizeInTs);
    }

    const char* GUIStyleScriptCompilerClassAttribute::GetName() const
    {
        return this->m_name.c_str();
    }


    void GUIStyleScriptCompilerClassAttribute::SetValue(const char* value, ptrdiff_t sizeInTs)
    {
        this->m_value.Assign(value, sizeInTs);
    }

    const char* GUIStyleScriptCompilerClassAttribute::GetValue() const
    {
        return this->m_value.c_str();
    }


    GUIStyleScriptCompilerClassAttribute & GUIStyleScriptCompilerClassAttribute::operator=(const GUIStyleScriptCompilerClassAttribute &other)
    {
        if (this != &other)
        {
            this->m_name  = other.GetName();
            this->m_value = other.GetValue();
        }

        return *this;
    }


    ///////////////////////////////////////////////
    // GUIStyleScriptCompilerClassAttribute

    GUIStyleScriptCompilerClass::GUIStyleScriptCompilerClass()
        : m_name(),
          m_includes(),
          m_attributes(),
          m_subclasses()
    {
    }

    GUIStyleScriptCompilerClass::GUIStyleScriptCompilerClass(const GUIStyleScriptCompilerClass &other)
        : m_name(other.GetName()),
          m_includes(other.GetIncludes()),
          m_attributes(other.GetAttributes()),
          m_subclasses(other.GetSubClasses())
    {
    }

    GUIStyleScriptCompilerClass::~GUIStyleScriptCompilerClass()
    {
    }


    void GUIStyleScriptCompilerClass::SetName(const char* name, ptrdiff_t sizeInTs)
    {
        this->m_name.Assign(name, sizeInTs);
    }

    const char* GUIStyleScriptCompilerClass::GetName() const
    {
        return this->m_name.c_str();
    }


    void GUIStyleScriptCompilerClass::SetIncludes(const char* includes, ptrdiff_t sizeInTs)
    {
        this->m_includes.Assign(includes, sizeInTs);
    }

    const char* GUIStyleScriptCompilerClass::GetIncludes() const
    {
        return this->m_includes.c_str();
    }


    void GUIStyleScriptCompilerClass::AddAttribute(const GUIStyleScriptCompilerClassAttribute &attribute)
    {
        // Remove the existing attribute. Don't want to replace, because we want to keep the order.
        for (size_t iAttribute = 0; iAttribute < this->m_attributes.count; ++iAttribute)
        {
            if (GT::Strings::Equal(this->m_attributes[iAttribute].GetName(), attribute.GetName()))
            {
                this->m_attributes.Remove(iAttribute);
                break;
            }
        }

        // Add a copy of the attribute.
        this->m_attributes.PushBack(attribute);
    }

    void GUIStyleScriptCompilerClass::RemoveAttributeByIndex(size_t index)
    {
        this->m_attributes.Remove(index);
    }

    bool GUIStyleScriptCompilerClass::FindAttributeByName(const char* name, size_t &index) const
    {
        for (size_t iAttribute = 0; iAttribute < this->m_attributes.count; ++iAttribute)
        {
            if (GT::Strings::Equal(this->m_attributes[iAttribute].GetName(), name))
            {
                index = iAttribute;
                return true;
            }
        }

        return false;
    }

    size_t GUIStyleScriptCompilerClass::GetAttributeCount() const
    {
        return this->m_attributes.count;
    }

    GUIStyleScriptCompilerClassAttribute & GUIStyleScriptCompilerClass::GetAttributeByIndex(size_t index)
    {
        return this->m_attributes[index];
    }

    const GUIStyleScriptCompilerClassAttribute & GUIStyleScriptCompilerClass::GetAttributeByIndex(size_t index) const
    {
        return this->m_attributes[index];
    }


    GUIStyleScriptCompilerClass* GUIStyleScriptCompilerClass::GetSubClass(const char* typeName, ptrdiff_t sizeInTs)
    {
        auto iSubClass = this->m_subclasses.Find(typeName, sizeInTs);
        if (iSubClass != nullptr)
        {
            return &iSubClass->value;
        }

        return nullptr;
    }

    const GUIStyleScriptCompilerClass* GUIStyleScriptCompilerClass::GetSubClass(const char* typeName, ptrdiff_t sizeInTs) const
    {
        auto iSubClass = this->m_subclasses.Find(typeName, sizeInTs);
        if (iSubClass != nullptr)
        {
            return &iSubClass->value;
        }

        return nullptr;
    }

    void GUIStyleScriptCompilerClass::AddSubClass(const char* modifierName, ptrdiff_t modifierNameSizeInTs, const GUIStyleScriptCompilerClass &subClass)
    {
        auto existingSubClass = this->GetSubClass(modifierName, modifierNameSizeInTs);
        if (existingSubClass != nullptr)
        {
            existingSubClass->Merge(subClass);
        }
        else
        {
            this->m_subclasses.Add(modifierName, modifierNameSizeInTs, subClass);
        }
    }


    void GUIStyleScriptCompilerClass::Merge(const GUIStyleScriptCompilerClass &other)
    {
        // Attributes.
        {
            // Remove duplicates.
            auto &otherAttributes = other.GetAttributes();
            for (size_t iOtherAttribute = 0; iOtherAttribute < otherAttributes.count; ++iOtherAttribute)
            {
                auto &otherAttribute = otherAttributes[iOtherAttribute];

                size_t existingAttributeIndex;
                if (this->FindAttributeByName(otherAttribute.GetName(), existingAttributeIndex))
                {
                    this->RemoveAttributeByIndex(existingAttributeIndex);
                }
            }

            // Add all attributes from 'other'.
            for (size_t iOtherAttribute = 0; iOtherAttribute < otherAttributes.count; ++iOtherAttribute)
            {
                this->AddAttribute(otherAttributes[iOtherAttribute]);
            }
        }


        // Includes.
        {
            this->m_includes += " ";
            this->m_includes += other.GetIncludes();
        }
    }


    GUIStyleScriptCompilerClass & GUIStyleScriptCompilerClass::operator=(const GUIStyleScriptCompilerClass &other)
    {
        if (this != &other)
        {
            this->m_name       = other.GetName();
            this->m_includes   = other.GetIncludes();
            this->m_attributes = other.GetAttributes();
            this->m_subclasses = other.GetSubClasses();
        }

        return *this;
    }



    ///////////////////////////////////////////////
    // GUIStyleScriptCompilerVariable

    GUIStyleScriptCompilerVariable::GUIStyleScriptCompilerVariable()
        : m_name(),
          m_value()
    {
    }

    GUIStyleScriptCompilerVariable::GUIStyleScriptCompilerVariable(const GUIStyleScriptCompilerVariable &other)
        : m_name(other.GetName()),
          m_value(other.GetValue())
    {
    }


    void GUIStyleScriptCompilerVariable::SetName(const char* name, ptrdiff_t nameSizeInTs)
    {
        this->m_name.Assign(name, nameSizeInTs);
    }

    const char* GUIStyleScriptCompilerVariable::GetName() const
    {
        return this->m_name.c_str();
    }


    void GUIStyleScriptCompilerVariable::SetValue(const char* value, ptrdiff_t sizeInTs)
    {
        this->m_value.Assign(value, sizeInTs);
    }

    const char* GUIStyleScriptCompilerVariable::GetValue() const
    {
        return this->m_value.c_str();
    }


    GUIStyleScriptCompilerVariable & GUIStyleScriptCompilerVariable::operator=(const GUIStyleScriptCompilerVariable &other)
    {
        if (this != &other)
        {
            this->m_name  = other.GetName();
            this->m_value = other.GetValue();
        }

        return *this;
    }


    ///////////////////////////////////////////////
    // GUIStyleScriptCompiler

    GUIStyleScriptCompiler::GUIStyleScriptCompiler()
        : m_identifier(),
          m_classes(),
          m_variables(),
          m_errorHandler(nullptr)
    {
    }

    GUIStyleScriptCompiler::GUIStyleScriptCompiler(const char* identifier)
        : m_identifier(identifier),
          m_classes(),
          m_variables(),
          m_errorHandler(nullptr)
    {
    }
    
    GUIStyleScriptCompiler::GUIStyleScriptCompiler(const GUIStyleScriptCompiler &other)
        : m_identifier(other.m_identifier),
          m_classes(other.m_classes),
          m_variables(other.m_variables),
          m_errorHandler(other.m_errorHandler)
    {
    }

    GUIStyleScriptCompiler::~GUIStyleScriptCompiler()
    {
        for (size_t i = 0; i < m_classes.GetCount(); ++i)
        {
            delete m_classes[i];
        }
        
        for (size_t i = 0; i < m_variables.GetCount(); ++i)
        {
            delete m_variables[i];
        }
    }


    bool GUIStyleScriptCompiler::Compile(const char* script, const char* baseURLPath)
    {
        if (script != nullptr)
        {
            // Loading is simply a matter of iterating over the tokenizer. The tokenizer will handle things
            // like comments and syntax.
            GUIStyleTokenizer token(script);
            while (token)
            {
                if (token.Equals("var"))
                {
                    // We're loading a variable. Next token is the name.
                    if (++token)
                    {
                        auto variableNameStart = token.start;
                        auto variableNameEnd   = token.end;

                        // Next token must be a :
                        if (++token)
                        {
                            if (token.Equals(':'))
                            {
                                if (++token)
                                {
                                    // Now we just loop through until we find a \n, ; or reach the end. This will be value of the variable.
                                    auto variableValueStart = token.start;
                                    auto variableValueEnd   = token.end;

                                    while (token)
                                    {
                                        if (token.Equals('\n') || token.Equals(';'))
                                        {
                                            break;
                                        }

                                        variableValueEnd = token.end;
                                        ++token;
                                    }
                                    

                                    // Add the variable, replacing the old one if it exists.
                                    auto existingVariable = this->FindVariableByName(variableNameStart, variableNameEnd - variableNameStart);
                                    if (existingVariable != nullptr)
                                    {
                                        existingVariable->SetValue(variableValueStart, variableValueEnd - variableValueStart);
                                    }
                                    else
                                    {
                                        auto variable = new GUIStyleScriptCompilerVariable;
                                        variable->SetName(variableNameStart, variableNameEnd - variableNameStart);
                                        variable->SetValue(variableValueStart, variableValueEnd - variableValueStart);
                                        
                                        this->m_variables.PushBack(variable);
                                    }
                                }
                                else
                                {
                                    GUIStyleScriptError error;
                                    error.message    = "Unexpected end of script while reading variable. Expecting value. Format should be 'var <name>:<value>'.";
                                    error.lineString = token.GetCurrentLineString().c_str();
                                    error.lineNumber = token.GetCurrentLineNumber();
                                    this->PostError(error);

                                    return false;
                                }
                            }
                            else
                            {
                                GUIStyleScriptError error;
                                error.message.AssignFormatted("Unexpected Token '%s'. Expecting ':'", token.GetString().c_str());
                                error.lineString = token.GetCurrentLineString().c_str();
                                error.lineNumber = token.GetCurrentLineNumber();
                                this->PostError(error);

                                // This error is recoverable, so no early termination.
                                while (token)
                                {
                                    if (token.Equals('\n') || token.Equals(';'))
                                    {
                                        break;
                                    }

                                    ++token;
                                }
                            }
                        }
                        else
                        {
                            GUIStyleScriptError error;
                            error.message    = "Unexpected end of script while reading variable. Expecting ':'. Format should be 'var <name>:<value>'.";
                            error.lineString = token.GetCurrentLineString().c_str();
                            error.lineNumber = token.GetCurrentLineNumber();
                            this->PostError(error);

                            return false;
                        }
                    }
                    else
                    {
                        GUIStyleScriptError error;
                        error.message    = "Unexpected end of script while reading variable. Expecting name after 'var'. Format should be 'var <name>:<value>'.";
                        error.lineString = token.GetCurrentLineString().c_str();
                        error.lineNumber = token.GetCurrentLineNumber();
                        this->PostError(error);

                        return false;
                    }
                }
                else if (!token.Equals('\n'))
                {
                    // Assume this token is a class name.
                    GUIStyleScriptCompilerClass newClass;
                    newClass.SetName(token.start, token.end - token.start);

                    if (++token)
                    {
                        const char* subClassNameStart = nullptr;
                        const char* subClassNameEnd   = nullptr;

                        if (token.Equals("includes"))
                        {
                            // We doing an include list. We loop over each token here until we find an opening curly bracket or a colon. We skip over new lines.
                            if (++token)
                            {
                                // The string containing the includes.
                                GT::Strings::List<char> includes;

                                while (token)
                                {
                                    // Skip over new line.
                                    if (!token.Equals('\n'))
                                    {
                                        // No more includes.
                                        if (token.Equals('{') || token.Equals(':'))
                                        {
                                            break;
                                        }

                                        // If this include is not the first one, we need to seperate it from the previous include with a space.
                                        if (includes.root != nullptr)
                                        {
                                            includes.Append(" ");
                                        }

                                        includes.Append(token.start, token.end - token.start);
                                    }

                                    ++token;
                                }

                                // Set the includes on the new class.
                                newClass.SetIncludes(includes.c_str());

                                // If we don't have a token, it means we've hit an unexpected end to the script.
                                if (!token)
                                {
                                    GUIStyleScriptError error;
                                    error.message    = "Unexpected end of script while reading includes. Expecting '{' or ':'.";
                                    error.lineString = token.GetCurrentLineString().c_str();
                                    error.lineNumber = token.GetCurrentLineNumber();
                                    error.info = 
                                        "Example:\n"
                                        "MyClass includes MyParent0 MyParent1\n"
                                        "{\n"
                                        "    ...\n"
                                        "}";

                                    this->PostError(error);
                                    return false;
                                }
                            }
                            else
                            {
                                GUIStyleScriptError error;
                                error.message    = "Unexpected end of script while reading includes. Expecting parent class names.";
                                error.lineString = token.GetCurrentLineString().c_str();
                                error.lineNumber = token.GetCurrentLineNumber();
                                error.info = 
                                    "Example:\n"
                                    "MyClass includes MyParent0 MyParent1\n"
                                    "{\n"
                                    "    ...\n"
                                    "}";

                                this->PostError(error);
                                return false;
                            }
                        }

                        token.SkipNewLines();

                        if (token.Equals(':'))
                        {
                            // Sub-class.
                            if (++token)
                            {
                                subClassNameStart = token.start;
                                subClassNameEnd   = token.end;

                                // The next token should be the opening bracket. It's checked below, but we need to check
                                // that we haven't reached the end.
                                if (!(++token))
                                {
                                    GUIStyleScriptError error;
                                    error.message    = "Unexpected end of script while reading event class. Expecting parent class names. Expecting opening curly bracket '{' or 'includes'.";
                                    error.lineString = token.GetCurrentLineString().c_str();
                                    error.lineNumber = token.GetCurrentLineNumber();
                                    error.info = 
                                        "Example:\n"
                                        "MyClass : hovered\n"
                                        "{\n"
                                        "    ...\n"
                                        "}";

                                    this->PostError(error);
                                    return false;
                                }
                            }
                            else
                            {
                                GUIStyleScriptError error;
                                error.message    = "Unexpected end of script while reading event class. Expecting parent class names. Expecting event class name (e.g. 'hovered').";
                                error.lineString = token.GetCurrentLineString().c_str();
                                error.lineNumber = token.GetCurrentLineNumber();
                                error.info = 
                                    "Example:\n"
                                    "MyClass : hovered\n"
                                    "{\n"
                                    "    ...\n"
                                    "}";

                                this->PostError(error);
                                return false;
                            }
                        }

                        token.SkipNewLines();

                        if (token.Equals('{'))
                        {
                            // We now want to read the class. We don't yet care about whether or not it's a sub-class/modifier-class. All we want
                            // to do is read the class like normal.
                            //
                            // We need to keep looping until we hit a closing curly bracket.
                            if (++token)
                            {
                                // Keeps track of whether or not we're sitting on a closing bracket. We use this so we know when to break from
                                // the closing-bracket loop.
                                bool foundBracket = false;

                                while (token)
                                {
                                    if (token.Equals('}'))
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        // We're starting a new attribute. Skip anything invalid like new lines.
                                        if (!token.Equals('\n'))
                                        {
                                            // Now assume it's an attribute name.
                                            GUIStyleScriptCompilerClassAttribute attribute;
                                            attribute.SetName(token.start, token.end - token.start);

                                            if (++token)
                                            {
                                                token.SkipNewLines();

                                                if (token.Equals(':'))
                                                {
                                                    if (++token)
                                                    {
                                                        token.SkipNewLines();

                                                        // When loading a value, we need to ensure all occurances of 'url' values are translated into actual paths. Due to this translation, we
                                                        // will have to construct a new string.
                                                        GT::String value;

                                                        while (token)
                                                        {
                                                            if (GT::Strings::Equal(token.start, 3, "url", 3))   // This check will never go beyond a null-terminator. Thus, it's safe to specify an explicit size for token.start.
                                                            {
                                                                if (++token)
                                                                {
                                                                    if (token.Equals('('))
                                                                    {
                                                                        GT::Strings::List<char> url;

                                                                        while (++token)
                                                                        {
                                                                            if (token.Equals(')'))
                                                                            {
                                                                                break;
                                                                            }

                                                                            url.Append(token.start, token.end - token.start);
                                                                        }

                                                                        // We have the URL, so now it needs to translated into an absolute path and added to the value string.
                                                                        //Path translatedURL(url.c_str());
                                                                        //if (baseURLPath != nullptr) {
                                                                        //    translatedURL.MakeAbsolute(baseURLPath);
                                                                        //}

                                                                        char translatedURL[DRFS_MAX_PATH];
                                                                        if (baseURLPath != nullptr) {
                                                                            drpath_to_absolute(url.c_str(), baseURLPath, translatedURL, sizeof(translatedURL));
                                                                        } else {
                                                                            strcpy_s(translatedURL, sizeof(translatedURL), url.c_str());
                                                                        }

                                                                        // We may need to ensure we place a space between the existing value string and the url.
                                                                        if (value.GetLengthInTs() > 0)
                                                                        {
                                                                            value.Append(" ");
                                                                        }

                                                                        value.Append(translatedURL);
                                                                    }
                                                                    else
                                                                    {
                                                                        GUIStyleScriptError error;
                                                                        error.message.AssignFormatted("Unexpected token '%s' while reading URL. Expecting '('.", token.GetString().c_str());
                                                                        error.lineString = token.GetCurrentLineString().c_str();
                                                                        error.lineNumber = token.GetCurrentLineNumber();
                                                                        error.info = 
                                                                            "Example:\n"
                                                                            "MyClass\n"
                                                                            "{\n"
                                                                            "    background-image: url(path/to/my/image.png)\n"
                                                                            "}\n"
                                                                            "\n"
                                                                            "Do not include quotes in the path.";

                                                                        this->PostError(error);
                                                                        return false;
                                                                    }
                                                                }
                                                                else
                                                                {
                                                                    GUIStyleScriptError error;
                                                                    error.message    = "Unexpected end of script while reading URL. Expecting '('.";
                                                                    error.lineString = token.GetCurrentLineString().c_str();
                                                                    error.lineNumber = token.GetCurrentLineNumber();
                                                                    error.info = 
                                                                        "Example:\n"
                                                                        "MyClass\n"
                                                                        "{\n"
                                                                        "    background-image: url(path/to/my/image.png)\n"
                                                                        "}\n"
                                                                        "\n"
                                                                        "Do not include quotes in the path.";

                                                                    this->PostError(error);
                                                                    return false;
                                                                }
                                                            }
                                                            else
                                                            {
                                                                if (token.Equals('\n') || token.Equals(';'))
                                                                {
                                                                    break;
                                                                }
                                                                else if (token.Equals('}'))
                                                                {
                                                                    foundBracket = true;
                                                                    break;
                                                                }


                                                                if (value.GetLengthInTs() > 0)
                                                                {
                                                                    value.Append(" ");
                                                                }

                                                                value.Append(token.start, token.end - token.start);
                                                            }

                                                            ++token;
                                                        }

                                                        // The value can now be set.
                                                        attribute.SetValue(value.c_str());

                                                        // The name and value is set, so we can now add the attribute to the class.
                                                        newClass.AddAttribute(attribute);
                                                    }
                                                    else
                                                    {
                                                        GUIStyleScriptError error;
                                                        error.message    = "Unexpected end of script while reading class attribute. Expecting value.";
                                                        error.lineString = token.GetCurrentLineString().c_str();
                                                        error.lineNumber = token.GetCurrentLineNumber();
                                                        error.info = 
                                                            "Example:\n"
                                                            "MyClass\n"
                                                            "{\n"
                                                            "    width: 100%\n"
                                                            "}";

                                                        this->PostError(error);
                                                        return false;
                                                    }
                                                }
                                                else
                                                {
                                                    GUIStyleScriptError error;
                                                    error.message.AssignFormatted("Unexpected Token '%s'. Expecting ':'", token.GetString().c_str());
                                                    error.lineString = token.GetCurrentLineString().c_str();
                                                    error.lineNumber = token.GetCurrentLineNumber();
                                                    error.info =
                                                        "Expected Format: <name>:<value>\n"
                                                        "\n"
                                                        "Example:\n"
                                                        "MyClass\n"
                                                        "{\n"
                                                        "    width: 100%\n"
                                                        "}";

                                                    this->PostError(error);

                                                    // We continue on here since it's better to load any valid attributes after the fact.
                                                    while (token)
                                                    {
                                                        if (token.Equals('\n') || token.Equals(';'))
                                                        {
                                                            break;
                                                        }
                                                        else if (token.Equals('}'))
                                                        {
                                                            foundBracket = true;
                                                            break;
                                                        }

                                                        ++token;
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                // Unexpected end.
                                                return false;
                                            }
                                        }
                                    }

                                    // If we found the bracket, just break the loop without incrementing.
                                    if (foundBracket)
                                    {
                                        break;
                                    }

                                    ++token;
                                }

                                // If we don't have a token here, it means we reached the end too early.
                                if (!token)
                                {
                                    GUIStyleScriptError error;
                                    error.message    = "Unexpected end of script. Closing curly bracket was not found.";
                                    error.lineString = token.GetCurrentLineString().c_str();
                                    error.lineNumber = token.GetCurrentLineNumber();
                                    error.info = 
                                        "Example:\n"
                                        "MyClass\n"
                                        "{\n"
                                        "    ...\n"
                                        "}";

                                    this->PostError(error);
                                    return false;
                                }


                                // Add the style class.
                                //
                                // If we've made it here we need to add the style class. If it is a sub class, we need to grab a reference to the base class and add
                                // it to that. Otherwise we just add it to the main list.
                                if (subClassNameStart != nullptr)
                                {
                                    // It's a sub-class, so we need to get the base class and add it. If the base class does not exist, we'll just create an empty one.
                                    auto baseClass = this->FindClassByName(newClass.GetName());
                                    if (baseClass == nullptr)
                                    {
                                        auto newBaseClass = new GUIStyleScriptCompilerClass;
                                        newBaseClass->SetName(newClass.GetName());
                                                
                                        this->m_classes.PushBack(newBaseClass);
                                        baseClass = this->m_classes.GetBack();
                                    }

                                    assert(baseClass != nullptr);
                                    {
                                        baseClass->AddSubClass(subClassNameStart, subClassNameEnd - subClassNameStart, newClass);
                                    }
                                }
                                else
                                {
                                    // It's not a sub-class, so we just add it to the main list. If it already exists, we don't want to replace, but instead merge.
                                    auto existingClass = this->FindClassByName(newClass.GetName());
                                    if (existingClass != nullptr)
                                    {
                                        existingClass->Merge(newClass);
                                    }
                                    else
                                    {
                                        this->m_classes.PushBack(new GUIStyleScriptCompilerClass(newClass));
                                    }
                                }
                            }
                            else
                            {
                                GUIStyleScriptError error;
                                error.message    = "Unexpected end of script. Style class is not closed.";
                                error.lineString = token.GetCurrentLineString().c_str();
                                error.lineNumber = token.GetCurrentLineNumber();
                                error.info = 
                                    "Example:\n"
                                    "MyClass\n"
                                    "{\n"
                                    "    ...\n"
                                    "}";

                                this->PostError(error);
                                return false;
                            }
                        }
                        else
                        {
                            GUIStyleScriptError error;
                            error.message    = "Unexpected end of script. Expecting opening curly bracket '{'.";
                            error.lineString = token.GetCurrentLineString().c_str();
                            error.lineNumber = token.GetCurrentLineNumber();
                            error.info = 
                                "Example:\n"
                                "MyClass\n"
                                "{\n"
                                "    ...\n"
                                "}";

                            this->PostError(error);
                            return false;
                        }
                    }
                    else
                    {
                        GUIStyleScriptError error;
                        error.message    = "Unexpected end of script while reading style class.";
                        error.lineString = token.GetCurrentLineString().c_str();
                        error.lineNumber = token.GetCurrentLineNumber();
                        error.info = 
                            "Example:\n"
                            "MyClass\n"
                            "{\n"
                            "    ...\n"
                            "}";

                        this->PostError(error);
                        return false;
                    }
                }

                ++token;
            }

            return true;
        }

        return false;
    }


    const char* GUIStyleScriptCompiler::GetIdentifier() const
    {
        return this->m_identifier.c_str();
    }


    size_t GUIStyleScriptCompiler::GetClassCount() const
    {
        return this->m_classes.count;
    }

    GUIStyleScriptCompilerClass & GUIStyleScriptCompiler::GetClassByIndex(size_t index)
    {
        assert(m_classes[index] != nullptr);
        
        return *this->m_classes[index];
    }

    const GUIStyleScriptCompilerClass & GUIStyleScriptCompiler::GetClassByIndex(size_t index) const
    {
        assert(m_classes[index] != nullptr);
        
        return *this->m_classes[index];
    }

    GUIStyleScriptCompilerClass* GUIStyleScriptCompiler::FindClassByName(const char* name, ptrdiff_t nameLengthInTs)
    {
        for (size_t iClass = 0; iClass < this->m_classes.count; ++iClass)
        {
            assert(m_classes[iClass] != nullptr);
            
            if (GT::Strings::Equal(this->m_classes[iClass]->GetName(), name, nameLengthInTs))
            {
                return this->m_classes[iClass];
            }
        }

        return nullptr;
    }

    
    size_t GUIStyleScriptCompiler::GetVariableCount() const
    {
        return this->m_variables.count;
    }

    GUIStyleScriptCompilerVariable & GUIStyleScriptCompiler::GetVariableByIndex(size_t index)
    {
        assert(m_variables[index] != nullptr);
        
        return *m_variables[index];
    }

    const GUIStyleScriptCompilerVariable & GUIStyleScriptCompiler::GetVariableByIndex(size_t index) const
    {
        assert(m_variables[index] != nullptr);
        
        return *m_variables[index];
    }

    GUIStyleScriptCompilerVariable* GUIStyleScriptCompiler::FindVariableByName(const char* name, ptrdiff_t nameLengthInTs)
    {
        for (size_t iClass = 0; iClass < m_variables.count; ++iClass)
        {
            assert(m_variables[iClass] != nullptr);
            
            if (GT::Strings::Equal(m_variables[iClass]->GetName(), name, nameLengthInTs))
            {
                return m_variables[iClass];
            }
        }

        return nullptr;
    }


    GUIStyleScriptCompilerClassAttribute* GUIStyleScriptCompiler::GetClassAttribute(const char* className, const char* classModifierName, const char* attributeName)
    {
        auto compilerClass = this->FindClassByName(className);
        if (compilerClass != nullptr)
        {
            if (classModifierName != nullptr)
            {
                compilerClass = compilerClass->GetSubClass(classModifierName);

                // If the sub class isn't present, neither is the attribute.
                if (compilerClass == nullptr)
                {
                    return nullptr;
                }
            }

            size_t attributeIndex;
            if (compilerClass->FindAttributeByName(attributeName, attributeIndex))
            {
                return &compilerClass->GetAttributeByIndex(attributeIndex);
            }
        }

        return nullptr;
    }

    const GUIStyleScriptCompilerClassAttribute* GUIStyleScriptCompiler::GetClassAttribute(const char* className, const char* classModifierName, const char* attributeName) const
    {
        return const_cast<GUIStyleScriptCompiler*>(this)->GetClassAttribute(className, classModifierName, attributeName);
    }


    void GUIStyleScriptCompiler::SetErrorHandler(GUIStyleScriptCompilerErrorHandler* errorHandler)
    {
        this->m_errorHandler = errorHandler;
    }

    GUIStyleScriptCompilerErrorHandler* GUIStyleScriptCompiler::GetErrorHandler()
    {
        return this->m_errorHandler;
    }
    
    
    
    GUIStyleScriptCompiler & GUIStyleScriptCompiler::operator=(const GUIStyleScriptCompiler &other)
    {
        if (this != &other)
        {
            this->m_identifier   = other.m_identifier;
            this->m_classes      = other.m_classes;
            this->m_variables    = other.m_variables;
            this->m_errorHandler = other.m_errorHandler;
        }
        
        return *this;
    }
    


    /////////////////////////////////////////////
    // Private

    void GUIStyleScriptCompiler::PostError(const GUIStyleScriptError &error)
    {
        if (this->m_errorHandler != nullptr)
        {
            this->m_errorHandler->OnError(error);
        }
    }
}
