// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Scripting_hpp_
#define __GTLib_Scripting_hpp_

#include "Script.hpp"
#include <GTLib/Serializer.hpp>
#include <GTLib/Deserializer.hpp>

namespace GTLib
{
    namespace Scripting
    {
        /// Loads the GTLib script library into the given script.
        bool LoadGTScriptLibrary(Script &script);

        /// Creates and pushes a new Serializer object onto the top of the stack.
        ///
        /// @param serializer [in] A reference to the internal serializer object.
        void PushNewSerializer(GTLib::Script &script, GTLib::Serializer &serializer);

        /// Creates and pushes a new Deserializer object onto the top of the stack.
        ///
        /// @param deserializer [in] A reference to the internal deserializer object.
        void PushNewDeserializer(GTLib::Script &script, GTLib::Deserializer &deserializer);


        /// Structure containing the foreign function interface for the GT script library.
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
            int ToRelativePath(GTLib::Script &script);


            namespace IOFFI
            {
                /// Returns the file information of the given path.
                ///
                /// @param path [in] The path whose info is being retrieved.
                ///
                /// @remarks
                ///     This function should only be used internally. Thus, it starts with '__'
                int __GetFileInfo(GTLib::Script &script);

                /// Retrieves the path of the parent.
                ///
                /// @remarks
                ///     Put simply, this just removes the last item and slash from the path.
                int GetParentDirectoryPath(GTLib::Script &script);

                /// Retrieves the file name portion of the given path.
                ///
                /// @remarks
                ///     This just deletes every path segment except the last. This will also work for directories.
                int GetFileNameFromPath(GTLib::Script &script);

                /// Retrieves the extension of the given file name.
                int GetExtension(GTLib::Script &script);

                /// Returns a string with the extension of the given string removed.
                int RemoveExtension(GTLib::Script &script);

                /// Determines whether or not the given file exists.
                int FileExists(GTLib::Script &script);

                /// Creates a folder at the given path.
                int CreateDirectory(GTLib::Script &script);

                /// Deletes a folder at the given path.
                int DeleteDirectory(GTLib::Script &script);

                /// Creates an empty file at the given path.
                ///
                /// @remarks
                ///     This will not leave the file handle open.
                int CreateEmptyFile(GTLib::Script &script);

                /// Deletes a file at the given path.
                int DeleteFile(GTLib::Script &script);
            }

            namespace TimingFFI
            {
                /// Retrieves the current time in seconds.
                int GetTimeInSeconds(GTLib::Script &script);

                /// Retrieves the current time in milliseconds.
                int GetTimeInMilliseconds(GTLib::Script &script);
            }

            namespace SystemFFI
            {
                /// Opens a file or URL in the preferred application.
                ///
                /// @remarks
                ///     Argument 1: The URL or path of the file to open.
                int OpenFileOrURL(GTLib::Script &script);
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
                int WriteInt8(GTLib::Script &script);

                /// Serializes a 16-bit integer.
                ///
                /// @remarks
                ///     Argument 1: A pointer to the Serializer object.
                ///     Argument 2: The integer to serialize.
                int WriteInt16(GTLib::Script &script);

                /// Serializes a 32-bit integer.
                ///
                /// @remarks
                ///     Argument 1: A pointer to the Serializer object.
                ///     Argument 2: The integer to serialize.
                int WriteInt32(GTLib::Script &script);

                /// Serializes a 32-bit floating point number.
                ///
                /// @remarks
                ///     Argument 1: A pointer to the Serializer object.
                ///     Argument 2: The number fo serialize.
                int WriteFloat32(GTLib::Script &script);

                /// Serializes a 64-bit floating point number.
                ///
                /// @remarks
                ///     Argument 1: A pointer to the Serializer object.
                ///     Argument 2: The number fo serialize.
                int WriteFloat64(GTLib::Script &script);

                /// Serializes a boolean.
                ///
                /// @remarks
                ///     Argument 1: A pointer to the Serializer object.
                ///     Argument 2: The boolean to serialize.
                int WriteBool(GTLib::Script &script);

                /// Serializes a string using the Serializer::WriteString() method.
                ///
                /// @remarks
                ///     Argument 1: A pointer to the Serializer object.
                ///     Argument 2: The string to serialize.
                int WriteString(GTLib::Script &script);
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
                int CreateFromFile(GTLib::Script &script);

                /// Deletes the give deserializer object.
                ///
                /// @param
                ///     Argument 1: A pointer to the deserializer to delete.
                ///     Argument 2: The file handle to close.
                int DeleteFromFile(GTLib::Script &script);


                /// Deserializes an 8-bit integer.
                ///
                /// @remarks
                ///     Argument 1: A pointer to the Deserializer object.
                ///     Return: The deserialized integer.
                int ReadInt8(GTLib::Script &script);

                /// Deserializes a 16-bit integer.
                ///
                /// @remarks
                ///     Argument 1: A pointer to the Deserializer object.
                ///     Return: The deserialized integer.
                int ReadInt16(GTLib::Script &script);

                /// Deserializes a 32-bit integer.
                ///
                /// @remarks
                ///     Argument 1: A pointer to the Deserializer object.
                ///     Return: The deserialized integer.
                int ReadInt32(GTLib::Script &script);

                /// Deserializes a 32-bit floating point number.
                ///
                /// @remarks
                ///     Argument 1: A pointer to the Deserializer object.
                ///     Return: The deserialized number.
                int ReadFloat32(GTLib::Script &script);

                /// Deserializes a 64-bit floating point number.
                ///
                /// @remarks
                ///     Argument 1: A pointer to the Deserializer object.
                ///     Return: The deserialized number.
                int ReadFloat64(GTLib::Script &script);

                /// Deserializes a boolean.
                ///
                /// @remarks
                ///     Argument 1: A pointer to the Deserializer object.
                ///     Return: The deserialized boolean.
                int ReadBool(GTLib::Script &script);

                /// Deserializes a string using the Serializer::WriteString() method.
                ///
                /// @remarks
                ///     Argument 1: A pointer to the Deserializer object.
                ///     Return: The deserialized string
                int ReadString(GTLib::Script &script);


                /// Marks the beginning of the reading of a protected chunk of data.
                ///
                /// @remarks
                ///     Argument 1: A pointer to the Deserializer object.
                ///     Argument 2: The size in bytes of the chunk.
                int StartChunk(GTLib::Script &script);

                //// Marks the end of the reading of a protected chunk of data.
                ///
                /// @remarks
                ///     Argument 1: A pointer to the Deserializer object.
                ///     @par
                ///     If the current read position is not sitting at the end of the chunk, it will be seeked to it.
                int EndChunk(GTLib::Script &script);

                /// Determines whether or not there is room in the chunk for reading the given amount of bytes.
                ///
                /// @remarks
                ///     Argument 1: A pointer to the Deserializer object.
                ///     Argument 2: The number of bytes to check.
                int HasRoomInChunk(GTLib::Script &script);
            }


            namespace DebuggingFFI
            {
                /// Triggers a breakpoint.
                int Breakpoint(GTLib::Script &script);

                /// Prints the call stack.
                int PrintCallStack(GTLib::Script &script);
            }
        }
    }
}

#endif
