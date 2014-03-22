// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

#include <GTEngine/GamePackager.hpp>
#include <GTEngine/IO.hpp>
#include <GTLib/IO.hpp>
#include <GTLib/Path.hpp>

namespace GTEngine
{
    GamePackager::GamePackager(const char* outputDirectoryAbsolutePathIn)
        : outputDirectoryAbsolutePath(outputDirectoryAbsolutePathIn),
          dataDirectoryRelativePaths(),
          executableRelativePath()
    {
    }

    GamePackager::~GamePackager()
    {
    }


    void GamePackager::CopyDataDirectory(const char* sourceAbsolutePath, const char* destinationRelativePath)
    {
        GTLib::String directoryName = GTLib::IO::FileName(sourceAbsolutePath);

        bool isRootDataDirectory = false;
        if (destinationRelativePath == nullptr)
        {
            destinationRelativePath = GTLib::IO::FileName(sourceAbsolutePath);

            // When we hit this case, it means the directory is a root data directory. This will later on need to be part of the config, so we'll need
            // to keep track of it.
            this->dataDirectoryRelativePaths.PushBack(destinationRelativePath);
            isRootDataDirectory = true;
        }


        GTLib::IO::FileIterator iFile((GTLib::String(sourceAbsolutePath) + "/.*").c_str());
        while (iFile)
        {
            auto fileAbsolutePath = iFile.name;
            auto fileName         = GTLib::IO::FileName(fileAbsolutePath);

            if (iFile.isDirectory)
            {
                // Recursive. Don't want to copy over "var" directories.
                if (!(isRootDataDirectory && GTLib::Strings::Equal<false>(fileName, "var")))
                {
                    this->CopyDataDirectory((GTLib::String(sourceAbsolutePath) + "/" + fileName).c_str(), (GTLib::String(destinationRelativePath) + "/" + fileName).c_str());
                }
            }
            else
            {
                // It's a normal file.
                //
                // If the file is a model file that is not a .gtmodel, we need to check if it has an associated .gtmodel file that's newer. If so, we can
                // ignore the original model file.
                if (GTEngine::IO::IsSupportedModelExtension(fileName) && !GTLib::Path::ExtensionEqual(fileName, ".gtmodel"))
                {
                    // It's a non-gtmodel file. We need to look for an associated .gtmodel file.
                    GTLib::FileInfo gtmodelInfo;
                    if (GTLib::IO::GetFileInfo((GTLib::String(fileAbsolutePath) + ".gtmodel").c_str(), gtmodelInfo))
                    {
                        GTLib::FileInfo originalInfo;
                        if (GTLib::IO::GetFileInfo(fileAbsolutePath, originalInfo))
                        {
                            if (gtmodelInfo.lastModifiedTime > originalInfo.lastModifiedTime)
                            {
                                // We want to skip this file.
                                ++iFile;
                                continue;
                            }
                        }
                    }
                }

                this->CopyFile(fileAbsolutePath, (GTLib::String(destinationRelativePath) + "/" + fileName).c_str());
            }

            ++iFile;
        }
    }

    bool GamePackager::CopyExecutable(const char* sourceAbsolutePath, const char* destinationRelativePath)
    {
        // The first step is to just copy over the file like normal.
        bool successful = this->CopyFile(sourceAbsolutePath, destinationRelativePath);


        // We need to let the packager know where the main executable is so we can correctly build the 
        if (destinationRelativePath == nullptr)
        {
            destinationRelativePath = GTLib::IO::FileName(sourceAbsolutePath);
        }

        this->executableRelativePath = destinationRelativePath;


        return successful;
    }

    bool GamePackager::CopyFile(const char* sourceAbsolutePath, const char* destinationRelativePath)
    {
        if (destinationRelativePath == nullptr)
        {
            destinationRelativePath = GTLib::IO::FileName(sourceAbsolutePath);
        }

        return GTLib::IO::CopyFile(sourceAbsolutePath, (this->outputDirectoryAbsolutePath + "/" + destinationRelativePath).c_str());
    }


    bool GamePackager::WriteConfig()
    {
        if (!this->executableRelativePath.IsEmpty())
        {
            GTLib::Path executableDirectory(this->outputDirectoryAbsolutePath.c_str());
            executableDirectory.Append(executableRelativePath.c_str());
            executableDirectory.RemoveLast();            // <-- Remove the file name, leaving the executable directory.

            // With the executable directory determined we can determine the paths of the data directories.
            GTLib::Vector<GTLib::String> dataDirectoryConfigPaths;
            for (size_t iDataDirectory = 0; iDataDirectory < this->dataDirectoryRelativePaths.count; ++iDataDirectory)
            {
                GTLib::Path dataDirectoryAbsolutePath((this->outputDirectoryAbsolutePath + "/" + this->dataDirectoryRelativePaths[iDataDirectory]).c_str());
                dataDirectoryAbsolutePath.Clean();
                GTLib::Path dataDirectoryRelativePath(GTLib::IO::ToRelativePath(dataDirectoryAbsolutePath.c_str(), executableDirectory.c_str()).c_str());
                dataDirectoryRelativePath.Clean();

                dataDirectoryConfigPaths.PushBack(dataDirectoryRelativePath.c_str());
            }

            GTLib::Path configPath(executableDirectory.c_str());
            configPath.Append("config.lua");


            auto file = GTLib::IO::Open(configPath.c_str(), GTLib::IO::OpenMode::Write);
            if (file != nullptr)
            {
                for (size_t iDataDirectory = 0; iDataDirectory < dataDirectoryConfigPaths.count; ++iDataDirectory)
                {
                    auto index = static_cast<int>(iDataDirectory + 1);       // +1 because Lua is 1 based.
                    auto path  = dataDirectoryConfigPaths[iDataDirectory].c_str();

                    GTLib::IO::WriteString(file, GTLib::String::CreateFormatted("Directories.Data[%d] = \"%s\";", index, path).c_str());
                }


                GTLib::IO::Close(file);
                return true;
            }
        }

        return false;
    }
}
