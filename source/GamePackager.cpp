// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/GamePackager.hpp>
#include <GTGE/IO.hpp>
#include <GTGE/GTEngine.hpp>
#include <easy_path/easy_path.h>
#include <dr_libs/dr_vfs.h>

namespace GT
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
        String directoryName = easypath_file_name(sourceAbsolutePath);

        bool isRootDataDirectory = false;
        if (destinationRelativePath == nullptr)
        {
            destinationRelativePath = easypath_file_name(sourceAbsolutePath);

            // When we hit this case, it means the directory is a root data directory. This will later on need to be part of the config, so we'll need
            // to keep track of it.
            this->dataDirectoryRelativePaths.PushBack(destinationRelativePath);
            isRootDataDirectory = true;
        }


        drvfs_iterator iFile;
        if (drvfs_begin(g_Context->GetVFS(), sourceAbsolutePath, &iFile))
        {
            do
            {
                const char* fileAbsolutePath = iFile.info.absolutePath;
                const char* fileName         = easypath_file_name(fileAbsolutePath);

                if ((iFile.info.attributes & DRVFS_FILE_ATTRIBUTE_DIRECTORY) != 0)
                {
                    // Recursive. Don't want to copy over "var" directories.
                    if (!(isRootDataDirectory && Strings::Equal<false>(fileName, "var")))
                    {
                        this->CopyDataDirectory((String(sourceAbsolutePath) + "/" + fileName).c_str(), (String(destinationRelativePath) + "/" + fileName).c_str());
                    }
                }
                else
                {
                    // It's a normal file.
                    //
                    // If the file is a model file that is not a .gtmodel, we need to check if it has an associated .gtmodel file that's newer. If so, we can
                    // ignore the original model file.
                    if (GT::IsSupportedModelExtension(fileName) && !easypath_extension_equal(fileName, ".gtmodel"))
                    {
                        // It's a non-gtmodel file. We need to look for an associated .gtmodel file.
                        drvfs_file_info gtmodelInfo;
                        if (drvfs_get_file_info(g_Context->GetVFS(), (String(fileAbsolutePath) + ".gtmodel").c_str(), &gtmodelInfo))
                        {
                            drvfs_file_info originalInfo;
                            if (drvfs_get_file_info(g_Context->GetVFS(), fileAbsolutePath, &originalInfo))
                            {
                                if (gtmodelInfo.lastModifiedTime > originalInfo.lastModifiedTime)
                                {
                                    // We want to skip this file.
                                    continue;
                                }
                            }
                        }
                    }

                    this->CopyFile(fileAbsolutePath, (String(destinationRelativePath) + "/" + fileName).c_str());
                }
            } while (drvfs_next(g_Context->GetVFS(), &iFile));
        }
    }

    bool GamePackager::CopyExecutable(const char* sourceAbsolutePath, const char* destinationRelativePath)
    {
        // The first step is to just copy over the file like normal.
        bool successful = this->CopyFile(sourceAbsolutePath, destinationRelativePath);


        // We need to let the packager know where the main executable is so we can correctly build the 
        if (destinationRelativePath == nullptr)
        {
            destinationRelativePath = easypath_file_name(sourceAbsolutePath);
        }

        this->executableRelativePath = destinationRelativePath;


        return successful;
    }

    bool GamePackager::CopyFile(const char* sourceAbsolutePath, const char* destinationRelativePath)
    {
        if (destinationRelativePath == nullptr)
        {
            destinationRelativePath = easypath_file_name(sourceAbsolutePath);
        }

        return drvfs_copy_file(g_Context->GetVFS(), sourceAbsolutePath, (this->outputDirectoryAbsolutePath + "/" + destinationRelativePath).c_str(), false);
    }


    bool GamePackager::WriteConfig()
    {
        if (!this->executableRelativePath.IsEmpty())
        {
            char executableDirectory[DRVFS_MAX_PATH];
            easypath_copy_and_append(executableDirectory, sizeof(executableDirectory), this->outputDirectoryAbsolutePath.c_str(), executableRelativePath.c_str());
            easypath_remove_file_name(executableDirectory);

            //Path executableDirectory(this->outputDirectoryAbsolutePath.c_str());
            //executableDirectory.Append(executableRelativePath.c_str());
            //executableDirectory.RemoveLast();            // <-- Remove the file name, leaving the executable directory.

            // With the executable directory determined we can determine the paths of the data directories.
            Vector<String> dataDirectoryConfigPaths;
            for (size_t iDataDirectory = 0; iDataDirectory < this->dataDirectoryRelativePaths.count; ++iDataDirectory)
            {
                //Path dataDirectoryAbsolutePath((this->outputDirectoryAbsolutePath + "/" + this->dataDirectoryRelativePaths[iDataDirectory]).c_str());
                //dataDirectoryAbsolutePath.Clean();

                char dataDirectoryAbsolutePath[DRVFS_MAX_PATH];
                easypath_append_and_clean(dataDirectoryAbsolutePath, sizeof(dataDirectoryAbsolutePath), this->outputDirectoryAbsolutePath.c_str(), this->dataDirectoryRelativePaths[iDataDirectory].c_str());

                //Path dataDirectoryRelativePath(IO::ToRelativePath(dataDirectoryAbsolutePath.c_str(), executableDirectory.c_str()).c_str());
                //dataDirectoryRelativePath.Clean();

                char dataDirectoryRelativePath[DRVFS_MAX_PATH];
                easypath_to_relative(dataDirectoryAbsolutePath, executableDirectory, dataDirectoryRelativePath, sizeof(dataDirectoryRelativePath));

                dataDirectoryConfigPaths.PushBack(dataDirectoryRelativePath);
            }

            //Path configPath(executableDirectory.c_str());
            //configPath.Append("config.lua");

            char configPath[DRVFS_MAX_PATH];
            easypath_copy_and_append(configPath, sizeof(configPath), executableDirectory, "config.cfg");

            drvfs_file* pFile = drvfs_open(g_Context->GetVFS(), configPath, DRVFS_WRITE, 0);
            if (pFile != nullptr)
            {
                for (size_t iDataDirectory = 0; iDataDirectory < dataDirectoryConfigPaths.count; ++iDataDirectory)
                {
                    auto index = static_cast<int>(iDataDirectory + 1);       // +1 because Lua is 1 based.
                    auto path  = dataDirectoryConfigPaths[iDataDirectory].c_str();

                    drvfs_write_string(pFile, String::CreateFormatted("BaseDirectory \"%s\"\n", index, path).c_str());
                }


                drvfs_close(pFile);
                return true;
            }
        }

        return false;
    }
}
