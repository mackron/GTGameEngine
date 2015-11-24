// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Script_hpp_
#define __GTLib_Script_hpp_

#include <cstddef>
#include <GTLib/Vector.hpp>
#include <GTLib/String.hpp>
#include <GTLib/Strings/LineIterator.hpp>
#include <easy_fs/easy_vfs.h>

namespace GTLib
{
    class Script;


    /// Information about an item on the call stack.
    struct ScriptCallstackItem
    {
        /// Constructor.
        ScriptCallstackItem()
            : sourceName(), lineNumber(0), functionName(), functionTypeName()
        {
        }


        /// The name of the file or chunk of the script. This will usually be a file name or blank.
        GTLib::String sourceName;

        /// The line number of where the function was called.
        unsigned int lineNumber;

        /// The name of the function.
        GTLib::String functionName;

        /// The function type name ("global", "local", "method", "field" or "").
        GTLib::String functionTypeName;
    };


    /// Class used for handling errors from a script.
    class ScriptErrorHandler
    {
    public:

        /// Constructor.
        ScriptErrorHandler();

        /// Destructor.
        virtual ~ScriptErrorHandler();


        ///////////////////////////////////////////////////////
        // Events.

        /// Called when an error occurs inside the script.
        ///
        /// @param script  [in] A reference to the script to posted the error.
        /// @param message [in] The error message.
        virtual void OnError(Script &script, const char* message);

        /// Called when a syntax error occurs when loading the script.
        ///
        /// @param script     [in] A reference to the script where the error has occured.
        /// @param lineNumber [in] The line number.
        /// @param message    [in] The error message.
        virtual void OnSyntaxError(Script &script, int lineNumber, const char* message);

        /// Called when a runtime error occurs in the given script.
        ///
        /// @param script     [in] A reference to the script where the error has occured.
        /// @param sourceName [in] The name of the script chunk that this error originates from. This is usually a file name.
        /// @param lineNumber [in] The line number where the error occured.
        /// @param message    [in] The error message.
        /// @param callstack  [in] The callstack.
        virtual void OnRuntimeError(Script &script, const char* sourceName, int lineNumber, const char* message, const GTLib::Vector<ScriptCallstackItem> &callstack);
    };


    /// Class used for parsing script error messages.
    class ScriptErrorMessageIterator
    {
    public:

        /// Constructor.
        ScriptErrorMessageIterator(const char* message);

        /// Copy constructor.
        ScriptErrorMessageIterator(const ScriptErrorMessageIterator &other);

        /// Destructor.
        ~ScriptErrorMessageIterator();


        /// Pre-increment operator.
        ScriptErrorMessageIterator & operator++();

        /// Post-increment operator.
        ScriptErrorMessageIterator operator++(int)
        {
            ScriptErrorMessageIterator temp(*this);
            ++(*this);
            return temp;
        }

        /// Assignment operator.
        ScriptErrorMessageIterator & operator=(const ScriptErrorMessageIterator &other)
        {
            if (this != &other)
            {
                this->lineNumber   = other.lineNumber;
                this->message      = other.message;
                this->lineIterator = other.lineIterator;
                this->isDone       = other.isDone;
            }

            return *this;
        }

        /// Boolean cast operator.
        operator bool() const
        {
            return !this->isDone;
        }



        /// The line number of the error.
        int lineNumber;

        /// The error message.
        GTLib::String message;


    private:

        /// The line iterator.
        Strings::LineIterator lineIterator;

        /// Whether or not the iterator is done.
        bool isDone;
    };


    // Return value should be the number of return values pushed onto the stack.
    typedef int (* script_function_proc)(GTLib::Script &);

    /**
    *   \brief  A class used for scripting.
    *
    *   A GTLib::Script is just a Lua script. It uses the Lua syntax and API paradigms.
    */
    class Script
    {
    public:

        /**
        *   \brief  Constructor.
        */
        Script();

        /**
        *   \brief  Destructor.
        */
        virtual ~Script();

        /**
        *   \brief              Loads a textual script but does not execute it.
        *   \param  script [in] The script as a string of text.
        "   \param  name   [in] The name to apply to the script for use with debugging.
        *   \return             True if the script is loaded successfully; false otherwise.
        *
        *   \remarks
        *       Execute the script with Script::Execute().
        */
        virtual bool Load(const char *script, const char* name = "");

        /**
        *   \brief              Loads a binary script, but does not execute it.
        *   \param  script [in] The pointer to the data containing the pre-compiled script.
        *   \param  size   [in] The size of the buffer pointed to by 'script'.
        *   \param  name   [in] The name of the script for debugging and errors.
        *   \return             True if the script is loaded successfully; false otherwise.
        *
        *   \remarks
        *       Execute the script with Script::Execute().
        */
        virtual bool LoadBinary(const void *script, size_t size, const char *name);

        /**
        *   \brief                Loads a script from a text file but does not execute it.
        *   \param  fileName [in] The name of the file to load the script from.
        *   \param                True if the file is loaded successfully; false otherwise.
        *
        *   \remarks
        *       Execute the script with Script::Execute().
        */
        virtual bool LoadFile(easyvfs_context* pVFS, const char *fileName);

        /**
        *   \brief  Executes the script.
        *
        *   \param  returnValueCount [in] The number of return values.
        *
        *   \return True if the script is executed successfully; False otherwise.
        */
        virtual bool Execute(int returnValueCount = -1);

        /**
        *   \brief                        Loads and executes the given script text.
        *   \param  script           [in] The script string to execute. <b>This is not a file name</b>.
        *   \param  name             [in] The name of the script chunk being executed. This will be used in debugging. Set to a filename, usually.
        *   \param  returnValueCount [in] The number of return values.
        *   \return                       True if the script is loaded and executed successfully; false otherwise.
        *
        *   \remarks
        *       Use LoadAndExecuteFile() to load and execute a text-based file.
        *       \par
        *       This is directly equivalent to:
        *       <code>
        *       return this->Load(someScript) && this->Execute();
        *       </code>
        */
        virtual bool Execute(const char* script, const char* name, int returnValueCount) { return this->LoadAndExecute(script, name, returnValueCount); }
        virtual bool Execute(const char* script, const char* name)                       { return this->Execute(script, name, -1); }
        virtual bool Execute(const char* script, int returnValueCount)                   { return this->Execute(script, "", returnValueCount); }
        virtual bool Execute(const char* script)                                         { return this->Execute(script, "", -1); }

        /**
        *   \brief                        Helper method for loading and executing a binary script.
        *   \param  script           [in] The pointer to the data containing the pre-compiled script.
        *   \param  size             [in] The size of the buffer pointed to by 'script'.
        *   \param  name             [in] The name of the script for debugging and errors.
        *   \param  returnValueCount [in] The number of return values.
        *   \return                       True if the script is loaded successfully; false otherwise.
        *
        *   \remarks
        *       This is directly equivalent to:
        *       <code>
        *       return this->LoadBinary(...) && this->Execute();
        *       </code>
        */
        virtual bool ExecuteBinary(const void* script, size_t size, const char* name, int returnValueCount = -1) { return this->LoadBinaryAndExecute(script, size, name, returnValueCount); }

        /**
        *   \brief                        Helper method for loading and executing a file.
        *   \param  filename         [in] The name of the file to load and execute.
        *   \param  returnValueCount [in] The number of return values.
        *   \return                       True if the file is loaded and executed successfully.
        *
        *   \remarks
        *       This is directly equivalent to:
        *       <code>
        *       return this->LoadFile(someFileName) && this->Execute();
        *       </code>
        */
        virtual bool ExecuteFile(easyvfs_context* pVFS, const char* filename, int returnValueCount = -1) { return this->LoadFileAndExecute(pVFS, filename, returnValueCount); }


        /**
        *   \brief                        Helper method for loading and executing a script.
        *   \param  script           [in] The script string to execute. <b>This is not a file name</b>.
        *   \param  name             [in] The name of the chunk for use in debugging.
        *   \param  returnValueCount [in] The number of return values.
        *   \return                       True if the script is loaded and executed successfully; false otherwise.
        *
        *   \remarks
        *       Use LoadAndExecuteFile() to load and execute a text-based file.
        */
        virtual bool LoadAndExecute(const char* script, const char* name, int returnValueCount);
                bool LoadAndExecute(const char* script, const char* name)     { return this->LoadAndExecute(script, name, -1); }
                bool LoadAndExecute(const char* script, int returnValueCount) { return this->LoadAndExecute(script, "", returnValueCount); }
                bool LoadAndExecute(const char* script)                       { return this->LoadAndExecute(script, "", -1); }

        /**
        *   \brief                        Helper method for loading and executing a binary script.
        *   \param  script           [in] The pointer to the data containing the pre-compiled script.
        *   \param  size             [in] The size of the buffer pointed to by 'script'.
        *   \param  name             [in] The name of the script for debugging and errors.
        *   \param  returnValueCount [in] The number of return values.
        *   \return                       True if the script is loaded successfully; false otherwise.
        */
        virtual bool LoadBinaryAndExecute(const void* script, size_t size, const char* name, int returnValueCount = -1);

        /**
        *   \brief                        Helper method for loading and executing a file.
        *   \param  filename         [in] The name of the file to load and execute.
        *   \param  returnValueCount [in] The number of return values.
        *   \return                       True if the file is loaded and executed successfully.
        */
        virtual bool LoadFileAndExecute(easyvfs_context* pVFS, const char* filename, int returnValueCount = -1);


        /// Loads the GT library.
        bool LoadGTLibrary();


        /**
        *   \brief                  Calls the function that was last pushed onto the script's stack.
        *   \param  numArgs    [in] The number of arguments that have been pushed onto the stack for this function.
        *   \param  numResults [in] The number of return values. Use -1 to have all return values pushed onto the stack.
        *   \return                 True if the function is called succesfully; zero (false) otherwise.
        *
        *   \remarks
        *       To call a particular function, the following protocol must be followed:
        *           - Push the function onto the stack.
        *           - Push the arguments onto the stack in order. The first argument is pushed first, the second
        *             argument is pushed second, etc.
        *       \par
        *       The return values from the function call are pushed onto the stack.
        *       \par
        *       The 'numArgs' parameter specifies how many arguments were pushed onto the stack for the function
        *       that needs to be called.
        *       \par
        *       All of the arguments and the function itself are popped from the stack.
        *       \par
        *       If an error occurs, the OnError() method will be called on every currently attached event handler.
        */
        bool Call(int numArgs, int numResults = -1);


        /**
        *   \brief                Registers a C function so that it can be called from within this script.
        *   \param  name     [in] The name of the function.
        *   \param  function [in] A pointer to the ScriptFunction that should be registered.
        */
        void RegisterFunction(const char *name, script_function_proc function);

        /**
        *   \brief                 Pushes a closure onto the stack.
        *   \param  function  [in] The C function for the closure.
        *   \param  itemCount [in] The number of items associated with the c function.
        *
        *   \remarks
        *       A closure is different to a C function in that it contains data that is associated with that function. Therefore:
        *           - function: A C function with no associated data.
        *           - closure:  A C function + associated data.
        *       \par
        *       Any associated data should be pushed onto the stack before calling this function. push_closure() will pop
        *       these values from the stack. The 'itemCount' parameter is used to determine how many items are associated with the
        *       function and how many items to pop from the stack.
        *       \par
        *       To get the function callable from inside the script, you need to push a global variable after calling this function.
        *       For example, to create a function that is callable from inside the script called "foo" which has 2 items associated
        *       with it, you would do something like this:
        *       \code
        *       push_integer(23);
        *       push_string("some item");
        *       push_closure(foo, 2);
        *       pop_global("foo");
        *       \endcode
        *       The above code first pushes the 2 items that should be associated with the function onto the stack. The first item
        *       is some integer and the second item is a string. We then push the closure onto the stack, specifying the name of
        *       the function which is declared as "int foo(GTLib::Script *)" (all C functions which need to be called from within the
        *       script must use the same signiture). We associate a name with the function be setting the global variable with the
        *       name of the function ("foo", in this case) through the pop_global() function.
        *       \par
        *       Associated items are refered to as up values. In the above example, the first item we pushed onto the stack is our
        *       first up value. The second is our second up value, and so on.
        *       \par
        *       From inside C, you can access the associated data by calling get_*() with the appropriate index. The index
        *       can be determined by calling get_up_value_index(). To grab the index of the first associated item, you would do
        *       get_up_value_index(1); the second associated item would be get_up_value_index(2), and so on.
        *       \par
        *       Arguments can be retrieved by using positive stack indices. The first argument will be at index 1. The number of
        *       arguments can be retrieved with get_top().
        */
        void PushClosure(script_function_proc function, int itemCount);


        /**
        *   \brief            Takes the value of a global variable and pushes that value onto the stack.
        *   \param  name [in] The name of the global variable whose value is to be pushed onto the stack.
        */
        void GetGlobal(const char *name);

        /**
        *   \brief            Sets the value of the specified global variable ('name') to that of the top item on the stack, and then pops that item from the stack.
        *   \param  name [in] The name of the global variable that will received the popped value.
        */
        void SetGlobal(const char *name);


        /**
        *   \brief                 Helper method for pushing the result of the given script onto the stack.
        *   \param  statement [in] The string containing the value that should be pushed onto the stack.
        *   \return                True if the value was pushed successfully; false otherwise.
        *
        *   \remarks
        *       This function simply loads and executes the statement "return <value>", which will push the result onto the stack.
        *       \par
        *       Don't forget to pop the result with Pop().
        */
        bool Get(const char* statement);

        /**
        *   \brief  Helper method for setting the value of a variable.
        */
        bool Set(const char* dest, int value);
        bool Set(const char* dest, double value);
        bool Set(const char* dest, bool value);
        bool Set(const char* dest, const char* value);


        /**
        *   \brief             Pushes a value onto the stack.
        *   \param  value [in] The value to push onto the stack.
        */
        void Push(int value);
        void Push(int64_t value);
        void Push(double value);
        void Push(bool value);
        void Push(const char *value);
        void Push(void *value);

        /**
        *   \brief  Pushes a nil value onto the stack.
        */
        void PushNil();

        /**
        *   \brief  Pushes a new, empty table to the top of the stack.
        */
        void PushNewTable();

        /**
        *   \brief                   Pushes a new userdata object onto the stack.
        *   \param  sizeInBytes [in] The size of the user data.
        *   \return                  A pointer to the user data memory.
        *
        *   \remarks
        *       Use a placement-new if you need to call a constructor.
        */
        void* PushNewUserData(size_t sizeInBytes);

        /**
        *   \brief             Pushes a copy of the value at the given index to the top of the stack.
        *   \param  index [in] The index of the item on the stack to push to the top.
        */
        void PushValue(int index);

        /**
        *   \brief             Pops a number of elements from the stack.
        *   \param  count [in] The number of values to pop from the stack.
        */
        void Pop(int count);

        /**
        *   \brief             Retrieves the value of an item on the stack
        *   \param  index [in] The index on the stack to retrieve the value from.
        *   \return            The value at the specified position on the stack.
        *
        *   \remarks
        *       If the index is negative, the value is retrieved from the top of the stack.
        *       \par
        *       If the value at the specified position is not of the specified type, a sensible value
        *       for that type is returned. An integer will return 0. A string will return an empty string.
        */
        int          ToInteger(int index);
        double       ToDouble(int index);
        float        ToFloat(int index);
        bool         ToBoolean(int index);
        const char * ToString(int index);
        void *       ToPointer(int index);
        void *       ToUserData(int index);


        /**
        *   \brief             Pushes the value of a table item onto the stack.
        *   \param  index [in] The index of the table on the stack that the value should be pushed from.
        *
        *   \remarks
        *       This function looks at the top item on the stack and extracts the value from the table
        *       based on that top item. An example for pushing the first item of a table follows:
        *       \code
        *       Push(1);                    // Lua is (annoyingly) 1 based, not 0 based.
        *       GetTableValue(-2);          // The table will be at -2 because we have just pushed a number.
        *       \endcode
        *       Below is another example show how to push the item in the table that has the key "some_number".
        *       \code
        *       Push("some_number");
        *       GetTableValue(-2);          // The table will be at -2 because we have just pushed a number.
        *       \endcode
        *       \par
        *       This function pops the key from the stack and replaces it with the value.
        */
        void GetTableValue(int tableIndex);

        /**
        *   \brief                  Pops the top value from the stack and places it in an item in a table.
        *   \param  tableIndex [in] The index of the table on the stack whose item will be set.
        *
        *   \remarks
        *       This function removes the top item from the stack and places it in the table item that
        *       has the key specified by the item just under the top item on the stack (at -2 on the stack).
        *       An example for setting the value with the key "some_number" with the value 17 follows.
        *       \code
        *       Push("some_number");
        *       Push(17);
        *       SetTableValue(-3);          // We have just pushed two values onto the stack, so the index can not be more than -3.
        *       \endcode
        *       \par
        *       This function pops both the key and value from the stack.
        */
        void SetTableValue(int tableIndex);

        /**
        *   \brief                  Moves to the next key/value pair in the table at the specified position.
        *   \param  tableIndex [in] The index on the stack where the table is located.
        *   \return                 True if there are more values in the table to iterate over.
        */
        bool Next(int tableIndex);


        /**
        *   \brief             Removes an item from the stack, moving other items as required.
        *   \param  index [in] The index of the item to remove.
        */
        void RemoveFromStack(int index);

        /**
        *   \brief             Moves the top item on the stack to the specified index position, moving other items as required.
        *   \param  index [in] The index to move the item to.
        */
        void InsertIntoStack(int index);

        /**
        *   \brief  Replaces the item at the given index with the value of the item at the top of the stack.
        */
        void Replace(int index);


        /**
        *   \brief             Determines if an item on the stack is nil or not.
        *   \param  index [in] The index of the item on the stack to check.
        *   \return            True if the item at the specified index is nil; false otherwise.
        */
        bool IsNil(int index);

        /**
        *   \brief             Determines if an item on the stack is a number.
        *   \param  index [in] The index of the item on the stack to check.
        *   \return            True if the item is a number; false otherwise.
        */
        bool IsNumber(int index);

        /**
        *   \brief             Determines if an item on the stack is a boolean.
        *   \param  index [in] The index of the item on the stack to check.
        *   \return            True if the item is a boolean; false otherwise.
        */
        bool IsBoolean(int index);

        /**
        *   \brief             Determines if an item on the stack is a stirng.
        *   \param  index [in] The index of the item on the stack to check.
        *   \return            True if the item is a string; false otherwise.
        */
        bool IsString(int index);

        /**
        *   \brief             Determines if an item on the stack is a table.
        *   \param  index [in] The index of the item on the stack to check.
        *   \return            True if the item is a table; false otherwise.
        */
        bool IsTable(int index);

        /**
        *   \brief             Determines if an item on the stack is a function.
        *   \param  index [in] The index of the item on the stack to check.
        *   \return            True if the item is a function; false otherwise.
        */
        bool IsFunction(int index);

        /**
        *   \brief             Determines if an item on the stack is a pointer.
        *   \param  index [in] The index of the item on the stack to check.
        *   \return            True if the item is a pointer; false otherwise.
        */
        bool IsPointer(int index);
        bool IsUserData(int index);

        /**
        *   \brief                  Simple function for retrieving the index of an up value.
        *   \param  localIndex [in] The local index of the up value whose actual index is being retrieved.
        *   \return                 The actual index of the up value.
        *
        *   \remarks
        *       To retrieve the first up value, you would do get_up_value_index(1). The second item would be get_up_value_index(2), and so on.
        */
        int GetUpValueIndex(int index);

        /**
        *   \brief             Ensures there is at least the specified number of slots available on the stack.
        *   \param  extra [in] The number of extra slots that must be available on the stack.
        *   \return            True if the stack has enough slots, and if required, has been resized successfully; false otherwise.
        */
        bool CheckStack(int extra);

        /**
        *   \brief  Retrieves the index of the top element on the stack.
        */
        int GetTop();

        /**
        *   \brief  Generates an error which can be captured from inside the script through the use of pcall.
        *
        *   \remarks
        *       The error message should be at the top of the stack. Can be any type. Internally, this function just calls
        *       lua_error(). lua_error() performs a long jump, meaning the function will never return. Make sure any objects
        *       are destructed correctly before calling this function.
        */
        void Error();

        /**
        *   \brief  Performs a full garbage-collection cycle.
        */
        void CollectGarbage();

        /**
        *   \brief             Pops the top item on the stack and sets it as the metatable of the item at the given index.
        *   \param  index [in] The index of the item whose having it's metatable set.
        */
        void SetMetaTable(int index);

        /// Pushes the metatable of item at the given index onto the stack.
        ///
        /// @param index [in] The index of the item whose metatable is being retrieved.
        ///
        /// @return True if the item has a metatable and it was pushed onto the stack; false otherwise.
        bool GetMetaTable(int index);

        /**
        *   \brief  Pushes a new metatable for userdata onto the stack.
        */
        void NewUserDataMetaTable(const char* mtName);

        /**
        *   \brief              Retrieves a userdata metatable based on it's name.
        *   \param  mtName [in] The name of the metatable to push onto the stack.
        */
        void GetUserDataMetaTable(const char* mtName);


        /// Helper method for retrieving a property from the config.
        ///
        /// @param propertyName [in] The name of the property as a string.
        ///
        /// @return The value of the property if it exists, or a logical default if it does not exist.
        ///
        /// @remarks
        ///     This method is a heavily simplified way of retrieving a value. If you need flexible retrievals with proper error handling, it is
        ///     best not to use this method.
        const char* GetString( const char* propertyName);
        int         GetInteger(const char* propertyName);
        double      GetDouble( const char* propertyName);
        float       GetFloat(  const char* propertyName) { return static_cast<float>(GetDouble(propertyName)); }
        bool        GetBoolean(const char* propertyName);


        /// Helper method for setting a value on a table.
        ///
        /// @param tableIndex [in] The index of the table.
        /// @param name       [in] The name of the property to set.
        /// @param value      [in] The value.
        template <typename T>
        void SetTableValue(int tableIndex, const char* name, T value)
        {
            if (tableIndex < 0)
            {
                tableIndex -= 2;
            }

            this->Push(name);
            this->Push(value);
            this->SetTableValue(tableIndex);
        }

        template <typename T, typename U>
        void SetTableValue(int tableIndex, T index, U value)
        {
            if (tableIndex < 0)
            {
                tableIndex -= 2;
            }

            this->Push(static_cast<int>(index));
            this->Push(value);
            this->SetTableValue(tableIndex);
        }

        /// Helper method for setting a function on a table.
        ///
        /// @param tableIndex   [in] The index of the table.
        /// @param functionName [in] The name of the function.
        /// @param function     [in] The function pointer.
        void SetTableFunction(int tableIndex, const char* functionName, script_function_proc function);
        void SetTableValue(int tableIndex, const char* name, script_function_proc value) { this->SetTableFunction(tableIndex, name, value); }

        /// Helper method for setting a pointer on a table.
        ///
        /// @param tableIndex [in] The index of the table.
        /// @param name       [in] The name of the property to set.
        /// @param value      [in] The value.
        void SetTablePointer(int tableIndex, const char* name, void* value);



        /// Attaches an error handler to the script.
        ///
        /// @param errorHandler [in] A reference to the error handler to attach to the script.
        void AttachErrorHandler(ScriptErrorHandler &errorHandler);

        /// Detaches the given error handler from the script.
        ///
        /// @param errorHandler [in] A reference to the error handler to detach from the script.
        void DetachErrorHandler(ScriptErrorHandler &errorHandler);



    public:

        /// Posts an error message to the error handlers.
        ///
        /// @param message [in] The error message.
        void OnError(const char* message);

        /// Posts a syntax error message to the error handlers.
        ///
        /// @param lineNumber [in] The line number of the error.
        /// @param message    [in] The error message.
        void OnSyntaxError(int lineNumber, const char* message);

        /// Posts a run-time error message to the error handlers.
        ///
        /// @param sourceName [in] The name of the script chunk that this error originates from. This is usually a file name.
        /// @param lineNumber [in] The line number where the error occured.
        /// @param message    [in] The error message.
        /// @param callstack  [in] The callstack.
        void OnRuntimeError(const char* sourceName, int lineNumber, const char* message, const GTLib::Vector<ScriptCallstackItem> &callstack);



        /// A hackly little function for retrieving the internal Lua state. This should not be used outside of GTLib.
        void* GetInernalLuaState() { return state; }




    private:

        /// The internal Lua_state object. We use a void* so that we don't have to include the Lua headers publicly.
        void* state;

        /// The list of error handlers that are currently attached to the script.
        GTLib::Vector<ScriptErrorHandler*> errorHandlers;


    private:    // No copying.
        Script(const Script &);
        Script & operator=(const Script &);
    };
}


#endif
