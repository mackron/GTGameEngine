// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Scripting_hpp_
#define __GTEngine_Scripting_hpp_

#include <GTEngine/Core/Serializer.hpp>
#include <GTEngine/Core/Deserializer.hpp>
#include <GTEngine/Script.hpp>
#include "Scripting/Scripting_Game.hpp"
#include "Scripting/Scripting_Math.hpp"
#include "Scripting/Scripting_Rendering.hpp"
#include "Scripting/Scripting_Audio.hpp"
#include "Scripting/Scripting_Scene.hpp"
#include "Scripting/Scripting_SceneNode.hpp"
#include "Scripting/Scripting_Components.hpp"
#include "Scripting/Scripting_Particles.hpp"
#include "Scripting/Scripting_Editor.hpp"
#include "Scripting/Scripting_Animation.hpp"

namespace GT
{
    // TODO: Combine these two functions into one: LoadGTScriptLibrary().

    /// Loads the core GT script library into the given script.
    bool LoadGTCoreScriptLibrary(Script &script);

    /// Loads the GTEngine script library into the given script.
    ///
    /// @param script [in] The script to load the GTEngine script library into.
    ///
    /// @return True if the library is loaded successfully; false otherwise.
    ///
    /// @remarks
    ///     This only loads the content in the GTEngine namespace/table. This will NOT load any game-specific functionality.
    bool LoadGTEngineScriptLibrary(GT::Script &script);




    /// Creates and pushes a new Serializer object onto the top of the stack.
    ///
    /// @param serializer [in] A reference to the internal serializer object.
    void PushNewSerializer(Script &script, Serializer &serializer);

    /// Creates and pushes a new Deserializer object onto the top of the stack.
    ///
    /// @param deserializer [in] A reference to the internal deserializer object.
    void PushNewDeserializer(Script &script, Deserializer &deserializer);




    /// Post a global MouseMove event to everything relevant.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param mousePosX [in] The x mouse position.
    /// @param mousePosY [in] The y mouse position.
    void PostEvent_OnMouseMove(GT::Script &script, int mousePosX, int mousePosY);

    /// Post a global MouseWheel event to everything relevant.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param mousePosX [in] The x mouse position.
    /// @param mousePosY [in] The y mouse position.
    /// @param delta     [in] The scroll delta.
    void PostEvent_OnMouseWheel(GT::Script &script, int mousePosX, int mousePosY, int delta);

    /// Post a global MouseButtonDown event to everything relevant.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param mousePosX [in] The x mouse position.
    /// @param mousePosY [in] The y mouse position.
    /// @param button    [in] The button code.
    void PostEvent_OnMouseButtonDown(GT::Script &script, int mousePosX, int mousePosY, MouseButton button);

    /// Post a global MouseButtonUp event to everything relevant.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param mousePosX [in] The x mouse position.
    /// @param mousePosY [in] The y mouse position.
    /// @param button    [in] The button code.
    void PostEvent_OnMouseButtonUp(GT::Script &script, int mousePosX, int mousePosY, MouseButton button);

    /// Post a global MouseButtonDoubleClick event to everything relevant.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param mousePosX [in] The x mouse position.
    /// @param mousePosY [in] The y mouse position.
    /// @param button    [in] The button code.
    void PostEvent_OnMouseButtonDoubleClick(GT::Script &script, int mousePosX, int mousePosY, MouseButton button);

    /// Post a global KeyPressed event to everything relevant.
    ///
    /// @param script [in] A reference to the main script object.
    /// @param key    [in] The key code.
    void PostEvent_OnKeyPressed(GT::Script &script, Key key);

    /// Post a global KeyReleased event to everything relevant.
    ///
    /// @param script [in] A reference to the main script object.
    /// @param key    [in] The key code.
    void PostEvent_OnKeyReleased(GT::Script &script, Key key);

    /// Posts a global OnGamePause event to everything relevant.
    ///
    /// @param script [in] A reference to the main script object.
    void PostEvent_OnGamePause(GT::Script &script);

    /// Posts a global OnGamePause event to everything relevant.
    ///
    /// @param script [in] A reference to the main script object.
    void PostEvent_OnGameResume(GT::Script &script);


    ///////////////////////////////////////////////////////////////
    // Script Definitions

    /// Loads a scene node script, replacing the old one if it exists.
    ///
    /// @param script             [in] A reference to the script to load the scene node script into.
    /// @param scriptRelativePath [in] The relative path of the script.
    /// @param scriptString       [in] The actual script content.
    bool LoadScriptDefinition(GT::Script &script, const char* scriptRelativePath, const char* scriptString);

    /// Unloads a scene node script.
    ///
    /// @param script             [in] A reference to the script to unload the scene node script from.
    /// @param scriptRelativePath [in] The relative path of the script to unload.
    void UnloadScriptDefinition(GT::Script &script, const char* scriptRelativePath);



    namespace FFI
    {
        /// Retrieves the contents of the given directory.
        ///
        /// @remarks
        ///     Argument 1: The name of the directory whose contents are being retrieved.
        ///     Argument 2: The table that will receive the directory names.
        ///     Argument 3: The table that will receive the file names.
        ///     @par
        ///     The returned strings will be in alphabetical order, indexed by an integer.
        int GetDirectoryContents(Script &script);

        /// Converts the given absolute path to a relative path.
        ///
        /// @remarks
        ///     Argument 1: The path to convert to a relative path.
        ///     Argument 2: The path to be relative to.
        int ToRelativePath(Script &script);



        /// Retrieves the executable directory.
        int GetExecutableDirectory(GT::Script &script);

        /// Retrieves the version string.
        int GetVersionString(GT::Script &script);

        /// Determines if the given file path is a model file, based on it's extension.
        int IsModelFile(GT::Script &script);

        /// Determines if the given file path is an image file, based on it's extension.
        int IsImageFile(GT::Script &script);

        /// Determines if the given file path is a sound file, based on it's extension.
        int IsSoundFile(GT::Script &script);

        /// Determines if the given file path is a scene file, based on it's extension.
        int IsSceneFile(GT::Script &script);

        /// Determines if the given file path is a prefab file, based on it's extension.
        int IsPrefabFile(GT::Script &script);

        /// Determines if the given file path is a script file, based on it's extension.
        int IsScriptFile(GT::Script &script);


        /// Creates a prefab file from a scene node.
        ///
        /// @remarks
        ///     Argument 1: The absolute path of the file to create or replace.
        ///     Argument 2: The path to make the absolute path relative to.
        ///     Argument 3: A pointer to the scene node to create the prefrab file from.
        int CreatePrefab(GT::Script &script);


        /// Executes the script defined in the given file.
        ///
        /// @remarks
        ///     Argument 1: The name of the script file to load and execute.
        int ExecuteFile(GT::Script &script);

        /// Executes the given script text.
        ///
        /// @remarks
        ///     Argument 1: The script text to execute.
        int ExecuteScript(GT::Script &script);

        /// Retrieves the last script error.
        int GetLastScriptError(GT::Script &script);


        /// Generates a random number between the two given numbers.
        ///
        /// @remarks
        ///     Argument 1: The low bound.
        ///     Argument 2: The high bound.
        int RandomInteger(GT::Script &script);
        int RandomFloat(GT::Script &script);





        namespace IOFFI
        {
            /// Returns the file information of the given path.
            ///
            /// @param path [in] The path whose info is being retrieved.
            ///
            /// @remarks
            ///     This function should only be used internally. Thus, it starts with '__'
            int __GetFileInfo(Script &script);

            /// Retrieves the path of the parent.
            ///
            /// @remarks
            ///     Put simply, this just removes the last item and slash from the path.
            int GetParentDirectoryPath(Script &script);

            /// Retrieves the file name portion of the given path.
            ///
            /// @remarks
            ///     This just deletes every path segment except the last. This will also work for directories.
            int GetFileNameFromPath(Script &script);

            /// Retrieves the extension of the given file name.
            int GetExtension(Script &script);

            /// Returns a string with the extension of the given string removed.
            int RemoveExtension(Script &script);

            /// Determines whether or not the given file exists.
            int FileExists(Script &script);

            /// Creates a folder at the given path.
            int CreateDirectory(Script &script);

            /// Deletes a folder at the given path.
            int DeleteDirectory(Script &script);

            /// Creates an empty file at the given path.
            ///
            /// @remarks
            ///     This will not leave the file handle open.
            int CreateEmptyFile(Script &script);

            /// Deletes a file at the given path.
            int DeleteFile(Script &script);
        }

        namespace TimingFFI
        {
            /// Retrieves the current time in seconds.
            int GetTimeInSeconds(Script &script);

            /// Retrieves the current time in milliseconds.
            int GetTimeInMilliseconds(Script &script);
        }

        namespace SystemFFI
        {
            /// Opens a file or URL in the preferred application.
            ///
            /// @remarks
            ///     Argument 1: The URL or path of the file to open.
            int OpenFileOrURL(Script &script);
        }



        ////////////////////////////////////
        // Serializer

        namespace SerializerFFI
        {
            /// Serializes an 8-bit integer.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Serializer object.
            ///     Argument 2: The integer to serialize.
            int WriteInt8(Script &script);

            /// Serializes a 16-bit integer.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Serializer object.
            ///     Argument 2: The integer to serialize.
            int WriteInt16(Script &script);

            /// Serializes a 32-bit integer.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Serializer object.
            ///     Argument 2: The integer to serialize.
            int WriteInt32(Script &script);

            /// Serializes a 32-bit floating point number.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Serializer object.
            ///     Argument 2: The number fo serialize.
            int WriteFloat32(Script &script);

            /// Serializes a 64-bit floating point number.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Serializer object.
            ///     Argument 2: The number fo serialize.
            int WriteFloat64(Script &script);

            /// Serializes a boolean.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Serializer object.
            ///     Argument 2: The boolean to serialize.
            int WriteBool(Script &script);

            /// Serializes a string using the Serializer::WriteString() method.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Serializer object.
            ///     Argument 2: The string to serialize.
            int WriteString(Script &script);
        }


        ////////////////////////////////////
        // Deserializer

        namespace DeserializerFFI
        {
            /// Creates and returns a pointer to a file deserializer.
            ///
            /// @remarks
            ///     Argument 1: The path of the file.
            ///     Return 1:   A pointer to the file deserializer.
            ///     Return 2:   The file handle.
            int CreateFromFile(Script &script);

            /// Deletes the give deserializer object.
            ///
            /// @param
            ///     Argument 1: A pointer to the deserializer to delete.
            ///     Argument 2: The file handle to close.
            int DeleteFromFile(Script &script);


            /// Deserializes an 8-bit integer.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Deserializer object.
            ///     Return: The deserialized integer.
            int ReadInt8(Script &script);

            /// Deserializes a 16-bit integer.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Deserializer object.
            ///     Return: The deserialized integer.
            int ReadInt16(Script &script);

            /// Deserializes a 32-bit integer.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Deserializer object.
            ///     Return: The deserialized integer.
            int ReadInt32(Script &script);

            /// Deserializes a 32-bit floating point number.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Deserializer object.
            ///     Return: The deserialized number.
            int ReadFloat32(Script &script);

            /// Deserializes a 64-bit floating point number.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Deserializer object.
            ///     Return: The deserialized number.
            int ReadFloat64(Script &script);

            /// Deserializes a boolean.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Deserializer object.
            ///     Return: The deserialized boolean.
            int ReadBool(Script &script);

            /// Deserializes a string using the Serializer::WriteString() method.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Deserializer object.
            ///     Return: The deserialized string
            int ReadString(Script &script);


            /// Marks the beginning of the reading of a protected chunk of data.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Deserializer object.
            ///     Argument 2: The size in bytes of the chunk.
            int StartChunk(Script &script);

            /// Marks the end of the reading of a protected chunk of data.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Deserializer object.
            ///     @par
            ///     If the current read position is not sitting at the end of the chunk, it will be seeked to it.
            int EndChunk(Script &script);

            /// Determines whether or not there is room in the chunk for reading the given amount of bytes.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the Deserializer object.
            ///     Argument 2: The number of bytes to check.
            int HasRoomInChunk(Script &script);
        }


        namespace DebuggingFFI
        {
            /// Triggers a breakpoint.
            int Breakpoint(Script &script);

            /// Prints the call stack.
            int PrintCallStack(Script &script);
        }
    }
}

#endif
