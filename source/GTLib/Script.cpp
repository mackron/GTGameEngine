// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Scripting.hpp>
#include <GTLib/IO.hpp>

#include <GTLib/Strings/List.hpp>
#include <GTLib/Strings/Tokenizer.hpp>
#include <GTLib/ToString.hpp>
#include <GTLib/Parse.hpp>

// Temp.
#include <GTLib/Errors.hpp>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define LUA_STATE ((lua_State*)this->state)


namespace GTLib
{
    ScriptErrorHandler::ScriptErrorHandler()
    {
    }

    ScriptErrorHandler::~ScriptErrorHandler()
    {
    }

    void ScriptErrorHandler::OnError(GTLib::Script &, const char*)
    {
    }

    void ScriptErrorHandler::OnSyntaxError(Script &, int, const char*)
    {
    }

    void ScriptErrorHandler::OnRuntimeError(Script &, const char*, int, const char*, const GTLib::Vector<ScriptCallstackItem> &)
    {
    }



    /// ScriptErrorMessageIterator
    ScriptErrorMessageIterator::ScriptErrorMessageIterator(const char* messageIn)
        : lineNumber(0), message(""), lineIterator(messageIn), isDone(false)
    {
        ++(*this);          // <-- This will setup the initial iteration.
    }

    ScriptErrorMessageIterator::ScriptErrorMessageIterator(const ScriptErrorMessageIterator &other)
        : lineNumber(other.lineNumber), message(other.message), lineIterator(other.lineIterator), isDone(other.isDone)
    {
    }

    ScriptErrorMessageIterator::~ScriptErrorMessageIterator()
    {
    }

    ScriptErrorMessageIterator & ScriptErrorMessageIterator::operator++()
    {
        if (*this)
        {
            this->isDone = !this->lineIterator.operator bool();
            if (!this->isDone)
            {
                // Each error is formated as "0:1:2" where:
                //   0 = A brief string containing the contents of the line (useless)
                //   1 = The line number
                //   2 = The error message

                GTLib::String lineNumberPrefix("\"]:");
                GTLib::String line(this->lineIterator.start, this->lineIterator.end - this->lineIterator.start);

                auto lineNumberStart = GTLib::Strings::FindFirst(line.c_str(), lineNumberPrefix.c_str());
                if (lineNumberStart != nullptr)
                {
                    GTLib::Strings::Tokenizer iToken(lineNumberStart + lineNumberPrefix.GetLength(), ":");

                    // 1 - The line numer.
                    GTLib::String lineNumberStr(iToken.start, iToken.end - iToken.start);
                    this->lineNumber = GTLib::Parse<int>(lineNumberStr.c_str());
                    ++iToken;

                    // 2 - The error message.
                    this->message.Assign(iToken.start, iToken.end - iToken.start);
                    this->message.Trim();
                }

                ++lineIterator;
            }
        }

        return *this;
    }
}


namespace GTLib
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // Some hacky void* <-> function pointer casts to avoid warnings and make the code technically more
    // correct. The sizeof() comparisons are constant, so they should hopefully be stripped at compile
    // time. Any warnings about constant expressions in an if statement can be safely silenced here.
 
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127)   // conditional expression is constant
#endif

    inline script_function_proc function_cast(void* other)
    {
        if (sizeof(other) == sizeof(script_function_proc))
        {
            return *reinterpret_cast<script_function_proc *>(&other);
        }
        else
        {
            script_function_proc result = 0;
            
            if (sizeof(other) < sizeof(result))
            {
                memcpy(&result, &other, sizeof(other));
            }
            else
            {
                memcpy(&result, &other, sizeof(result));
            }
            
            return result;
        }
    }
    
    inline void* function_cast(script_function_proc other)
    {
        if (sizeof(other) == sizeof(script_function_proc))
        {
            return *reinterpret_cast<void **>(&other);
        }
        else
        {
            void* result = 0;
            
            if (sizeof(other) < sizeof(result))
            {
                memcpy(&result, &other, sizeof(other));
            }
            else
            {
                memcpy(&result, &other, sizeof(result));
            }
            
            return result;
        }
    }

#ifdef _MSC_VER
#pragma warning(pop)
#endif
    
    
    int GenericCFunction(lua_State* state)
    {
        // We need to grab the script_function_proc pointer from the stack. This pointer was associated with the C function
        // when it was registered. We also need to grab the "GT_Script" global variable which will contain our script object.
        auto void_proc = lua_touserdata(state, lua_upvalueindex(1));
        script_function_proc proc = function_cast(void_proc);
        Script* self = nullptr;

        lua_getglobal(state, "GT_Script");
        self = static_cast<Script*>(lua_touserdata(state, -1));
        lua_pop(state, 1);

        // We can now call our function like normal.
        if (self)
        {
            return proc(*self);
        }

        return 0;
    }

    int GT_OnError(Script &script)
    {
        auto errorMessage = script.ToString(1);
        assert(errorMessage != nullptr);
        {
            GTLib::String formattedMessage;
            formattedMessage += errorMessage;
            formattedMessage += "\n";


            GTLib::Vector<ScriptCallstackItem> callstack;

            lua_State* state = reinterpret_cast<lua_State*>(script.GetInernalLuaState());
            lua_Debug  entry;
            int        depth = 1;

            while (lua_getstack(state, depth, &entry))
            {
                int status = lua_getinfo(state, "Sln", &entry);
                assert(status != 0);
                {
                    (void)status;   // <-- Warning silencer for release builds.

                    ScriptCallstackItem item;
                    item.sourceName       = entry.source;
                    item.lineNumber       = entry.currentline;
                    item.functionName     = entry.name;
                    item.functionTypeName = entry.namewhat;
                    callstack.PushBack(item);

                    formattedMessage.AppendFormatted("    [source \"%s\"]:%d: %s\n", entry.source, entry.currentline, entry.name ? entry.name : "?");
                    ++depth;
                }
            }

            script.OnError(formattedMessage.c_str());


            // Now we're going to grab all of the required data to post a properly formatted runtime error. The original message will contain the line
            // number and error message, but not the full source path. The full source path will be specified in the last item in the callstack.
            GTLib::String sourceName = callstack.GetBack().sourceName;

            ScriptErrorMessageIterator iError(errorMessage);
            assert(iError);
            {
                script.OnRuntimeError(sourceName.c_str(), iError.lineNumber, iError.message.c_str(), callstack);
            }
        }

        return 0;
    }

    Script::Script()
        : state(luaL_newstate()), errorHandlers()
    {
        luaL_openlibs(LUA_STATE);

        // We need to set a global variable in the script which is equal to 'this'. We need this for calling C functions.
        lua_pushlightuserdata(LUA_STATE, this);
        lua_setglobal(LUA_STATE, "GT_Script");


        this->RegisterFunction("GT_OnError", GT_OnError);


        // This is where we'll initialise some debugging stuff. Note with this part how we do NOT want to use the Script abstraction and instead
        // call the Lua functions directly. The reason is that Script will try looking for the error function, but that's the exact function that
        // we're trying to get loaded.
        const char* debuggingScript =
            "function GT_PCallError(originalMessage)"
            "    local message = originalMessage;"
            ""
            "    GT_OnError(message);"
            ""
            "    return message;"
            "end;";

        if (luaL_loadbuffer(LUA_STATE, debuggingScript, std::strlen(debuggingScript), "") == 0)
        {
            if (lua_pcall(LUA_STATE, 0, LUA_MULTRET, 0) != 0)
            {
                lua_pop(LUA_STATE, 1);
            }
        }
    }

    Script::~Script()
    {
        lua_close(LUA_STATE);
    }

    bool Script::LoadFile(easyvfs_context* pVFS, const char* fileName)
    {
        // We don't use Lua's built in file loading API. Instead we use our own virtual file system.

        char* pFileData = easyvfs_open_and_read_text_file(pVFS, fileName, nullptr);
        if (pFileData != nullptr)
        {
            bool retValue = this->Load(pFileData, fileName);

            easyvfs_free(pFileData);
            return retValue;
        }
        else
        {
            char buffer[256];
            GTLib::IO::snprintf(buffer, 256, "Could not find file: %s", fileName);

            this->Push(buffer);
        }

        return false;
    }

    bool Script::Load(const char* script, const char* name)
    {
        if (script != nullptr)
        {
            int result = luaL_loadbuffer(LUA_STATE, script, std::strlen(script), name);
            if (result != 0)
            {
                if (result == LUA_ERRSYNTAX)
                {
                    auto errorMessage = this->ToString(-1);
                    assert(errorMessage != nullptr);
                    {
                        // Since this is a syntax error, we want to parse the error message.
                        ScriptErrorMessageIterator iError(errorMessage);
                        while (iError)
                        {
                            this->OnSyntaxError(iError.lineNumber, iError.message.c_str());
                            ++iError;
                        }

                        this->OnError(errorMessage);
                    }
                }
                else if (result == LUA_ERRMEM)
                {
                    this->OnError("Error loading script - Memory allocation error (LUA_ERRMEM).");
                }

                // Error message must be popped.
                this->Pop(1);
                return false;
            }

            return true;
        }

        this->OnError("Script::Load() - No input script.");
        return false;
    }

    bool Script::LoadBinary(const void* script, size_t size, const char *name)
    {
        return luaL_loadbuffer(LUA_STATE, static_cast<const char*>(script), size, name) == 0;
    }

    bool Script::Execute(int returnValueCount)
    {
        return this->Call(0, returnValueCount);
    }


    bool Script::LoadAndExecute(const char* script, const char* name, int returnValueCount)
    {
        return this->Load(script, name) && this->Execute(returnValueCount);
    }

    bool Script::LoadBinaryAndExecute(const void* script, size_t size, const char* name, int returnValueCount)
    {
        return this->LoadBinary(script, size, name) && this->Execute(returnValueCount);
    }

    bool Script::LoadFileAndExecute(easyvfs_context* pVFS, const char* filename, int returnValueCount)
    {
        return this->LoadFile(pVFS, filename) && this->Execute(returnValueCount);
    }


    bool Script::LoadGTLibrary()
    {
        return GTLib::Scripting::LoadGTScriptLibrary(*this);
    }


    bool Script::Call(int numArgs, int numResults)
    {
        // We need to insert the error handler function just above the calling function.
        int errfunc = -2 - numArgs;
        lua_getglobal(LUA_STATE, "GT_PCallError");
        lua_insert(LUA_STATE, errfunc);

        // We need to grab the size of the stack before the call so we know where the error function is located at the end of it.
        int stackSizeBeforeCall = lua_gettop(LUA_STATE);

        int result = lua_pcall(LUA_STATE, numArgs, numResults, errfunc);
        if (result != 0)
        {
            // There are two values we need to pop here - the error message that pcall will have pushed and the error handler that we inserted earlier.
            this->Pop(2);
            return false;
        }


        // If we get here, everything executed fine, but we need to clean the stack by removing the error function. If we don't do this, we'll get a stack overflow.
        // The function will be located just before the return values. Thus, we'll need to find the number of return values.
        int returnValueCount = lua_gettop(LUA_STATE) - (stackSizeBeforeCall - 1 - numArgs);
        errfunc = -1 - returnValueCount;

        lua_remove(LUA_STATE, errfunc);

        return true;
    }

    void Script::RegisterFunction(const char* name, script_function_proc function)
    {
        // Because our function takes a script object instead of a lua_State pointer, we need to use the generic function,
        // but associate the function delegate with the C closure.
        this->PushClosure(function, 0);
        lua_setglobal(LUA_STATE, name);
    }

    void Script::PushClosure(script_function_proc function, int numItems)
    {
        // We need to insert our function pointer to the position just before our items.
        lua_pushlightuserdata(LUA_STATE, function_cast(function));
        lua_insert(LUA_STATE, -1 - numItems);

        // We can now push the closure. Because we need to also assocaite the function pointer with the function, we actually have 1 additional item.
        lua_pushcclosure(LUA_STATE, GenericCFunction, numItems + 1);
    }

    void Script::GetGlobal(const char* name)
    {
        lua_getglobal(LUA_STATE, name);
    }

    void Script::SetGlobal(const char* name)
    {
        lua_setglobal(LUA_STATE, name);
    }


    bool Script::Get(const char* statement)
    {
        Strings::List<char> command;
        command.Append("return ");
        command.Append(statement);

        return this->Load(command.c_str()) && this->Execute();
    }

    bool Script::Set(const char* dest, int value)
    {
        String valueStr = GTLib::ToString(value);

        Strings::List<char> command;
        command.Append(dest);
        command.Append("=");
        command.Append(valueStr.c_str());
        command.Append(";");

        return this->Load(command.c_str()) && this->Execute();
    }

    bool Script::Set(const char* dest, double value)
    {
        String valueStr = GTLib::ToString(value);

        Strings::List<char> command;
        command.Append(dest);
        command.Append("=");
        command.Append(valueStr.c_str());
        command.Append(";");

        return this->Load(command.c_str()) && this->Execute();
    }

    bool Script::Set(const char* dest, bool value)
    {
        Strings::List<char> command;
        command.Append(dest);
        command.Append("=");
        command.Append(value ? "true;" : "false;");

        return this->Load(command.c_str()) && this->Execute();
    }

    bool Script::Set(const char* dest, const char* value)
    {
        Strings::List<char> command;
        command.Append(dest);
        command.Append("='");
        command.Append(value);
        command.Append("';");

        return this->Load(command.c_str()) && this->Execute();
    }


    void Script::Push(int value)
    {
        lua_pushinteger(LUA_STATE, value);
    }
    void Script::Push(int64_t value)
    {
        lua_pushinteger(LUA_STATE, static_cast<lua_Integer>(value));
    }

    void Script::Push(double value)
    {
        lua_pushnumber(LUA_STATE, value);
    }

    void Script::Push(bool value)
    {
        lua_pushboolean(LUA_STATE, value);
    }

    void Script::Push(const char* value)
    {
        lua_pushstring(LUA_STATE, value);
    }

    void Script::Push(void* value)
    {
        lua_pushlightuserdata(LUA_STATE, value);
    }

    void Script::PushNil()
    {
        lua_pushnil(LUA_STATE);
    }

    void Script::PushNewTable()
    {
        lua_newtable(LUA_STATE);
    }

    void  *Script::PushNewUserData(size_t sizeInBytes)
    {
        return lua_newuserdata(LUA_STATE, sizeInBytes);
    }

    void Script::PushValue(int index)
    {
        lua_pushvalue(LUA_STATE, index);
    }


    void Script::Pop(int count)
    {
        lua_pop(LUA_STATE, count);
    }


    int Script::ToInteger(int index)
    {
        return static_cast<int>(lua_tointeger(LUA_STATE, index));
    }

    double Script::ToDouble(int index)
    {
        return lua_tonumber(LUA_STATE, index);
    }

    float Script::ToFloat(int index)
    {
        return static_cast<float>(this->ToDouble(index));
    }

    bool Script::ToBoolean(int index)
    {
        return lua_toboolean(LUA_STATE, index) != 0;
    }

    const char* Script::ToString(int index)
    {
        return lua_tostring(LUA_STATE, index);
    }

    void* Script::ToPointer(int index)
    {
        return lua_touserdata(LUA_STATE, index);
    }

    void* Script::ToUserData(int index)
    {
        return lua_touserdata(LUA_STATE, index);
    }


    void Script::GetTableValue(int index)
    {
        lua_gettable(LUA_STATE, index);
    }

    void Script::SetTableValue(int index)
    {
        lua_settable(LUA_STATE, index);
    }

    bool Script::Next(int tableIndex)
    {
        return lua_next(LUA_STATE, tableIndex) != 0;
    }


    void Script::RemoveFromStack(int index)
    {
        lua_remove(LUA_STATE, index);
    }

    void Script::InsertIntoStack(int index)
    {
        lua_insert(LUA_STATE, index);
    }

    void Script::Replace(int index)
    {
        lua_replace(LUA_STATE, index);
    }


    bool Script::IsNil(int index)
    {
        return lua_isnil(LUA_STATE, index) != 0;
    }

    bool Script::IsNumber(int index)
    {
        return lua_isnumber(LUA_STATE, index) != 0;
    }

    bool Script::IsBoolean(int index)
    {
        return lua_isboolean(LUA_STATE, index) != 0;
    }

    bool Script::IsString(int index)
    {
        return lua_isstring(LUA_STATE, index) != 0;
    }

    bool Script::IsTable(int index)
    {
        return lua_istable(LUA_STATE, index) != 0;
    }

    bool Script::IsFunction(int index)
    {
        return lua_isfunction(LUA_STATE, index) != 0;
    }

    bool Script::IsPointer(int index)
    {
        return lua_islightuserdata(LUA_STATE, index) != 0;
    }

    bool Script::IsUserData(int index)
    {
        return lua_isuserdata(LUA_STATE, index) != 0;
    }

    int Script::GetUpValueIndex(int localIndex)
    {
        // Every closure in our scripts contain a sort of hidden item which is always the first item. We
        // therefore need to add 1 to the local index to get passed this item.
        return lua_upvalueindex(localIndex + 1);
    }

    bool Script::CheckStack(int extra)
    {
        return lua_checkstack(LUA_STATE, extra) != 0;
    }

    int Script::GetTop()
    {
        return lua_gettop(LUA_STATE);
    }

    void Script::Error()
    {
        lua_error(LUA_STATE);
    }

    void Script::CollectGarbage()
    {
        lua_gc(LUA_STATE, LUA_GCCOLLECT, 1);
    }

    void Script::SetMetaTable(int index)
    {
        lua_setmetatable(LUA_STATE, index);
    }

    bool Script::GetMetaTable(int index)
    {
        return lua_getmetatable(LUA_STATE, index) != 0;
    }

    void Script::NewUserDataMetaTable(const char* mtName)
    {
        luaL_newmetatable(LUA_STATE, mtName);
    }

    void Script::GetUserDataMetaTable(const char* mtName)
    {
        luaL_getmetatable(LUA_STATE, mtName);
    }


    const char* Script::GetString(const char* propertyName)
    {
        const char* result = nullptr;

        if (this->Get(propertyName))
        {
            if (this->IsString(-1))
            {
                result = this->ToString(-1);
            }

            this->Pop(1);
        }

        return result;
    }

    int Script::GetInteger(const char* propertyName)
    {
        int result = 0;

        if (this->Get(propertyName))
        {
            if (this->IsNumber(-1))
            {
                result = this->ToInteger(-1);
            }

            this->Pop(1);
        }

        return result;
    }

    double Script::GetDouble(const char* propertyName)
    {
        double result = 0.0;

        if (this->Get(propertyName))
        {
            if (this->IsNumber(-1))
            {
                result = this->ToDouble(-1);
            }

            this->Pop(1);
        }

        return result;
    }

    bool Script::GetBoolean(const char* propertyName)
    {
        bool result = false;

        if (this->Get(propertyName))
        {
            if (this->IsBoolean(-1))
            {
                result = this->ToBoolean(-1);
            }

            this->Pop(1);
        }

        return result;
    }


    void Script::SetTableFunction(int tableIndex, const char* name, script_function_proc function)
    {
        if (tableIndex < 0)
        {
            tableIndex -= 2;
        }

        this->Push(name);
        this->PushClosure(function, 0);
        this->SetTableValue(tableIndex);
    }

    void Script::SetTablePointer(int tableIndex, const char* name, void* value)
    {
        if (tableIndex < 0)
        {
            tableIndex -= 2;
        }

        this->Push(name);
        this->Push(value);
        this->SetTableValue(tableIndex);
    }



    void Script::AttachErrorHandler(ScriptErrorHandler &errorHandler)
    {
        if (!this->errorHandlers.Exists(&errorHandler))
        {
            this->errorHandlers.PushBack(&errorHandler);
        }
    }

    void Script::DetachErrorHandler(ScriptErrorHandler &errorHandler)
    {
        this->errorHandlers.RemoveFirstOccuranceOf(&errorHandler);
    }





    ////////////////////////////////////////////////
    // For Internal Use

    void Script::OnError(const char* message)
    {
        // If there are no error handlers, we'll just print the message to standard out.
        if (this->errorHandlers.count == 0)
        {
            printf("%s\n", message);
        }
        else
        {
            for (size_t i = 0; i < this->errorHandlers.count; ++i)
            {
                this->errorHandlers[i]->OnError(*this, message);
            }
        }
    }

    void Script::OnSyntaxError(int lineNumber, const char* message)
    {
        for (size_t i = 0; i < this->errorHandlers.count; ++i)
        {
            this->errorHandlers[i]->OnSyntaxError(*this, lineNumber, message);
        }
    }

    void Script::OnRuntimeError(const char* sourceName, int lineNumber, const char* message, const GTLib::Vector<ScriptCallstackItem> &callstack)
    {
        for (size_t i = 0; i < this->errorHandlers.count; ++i)
        {
            this->errorHandlers[i]->OnRuntimeError(*this, sourceName, lineNumber, message, callstack);
        }
    }
}
