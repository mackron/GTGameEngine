// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/DataFilesWatcher.hpp>
#include <GTEngine/GTEngine.hpp>
#include <easy_path/easy_path.h>

namespace GTEngine
{
    /// This function will be called by the watcher's thread.
    void DataFilesWatcher_CheckForChanges_Async(void* self)
    {
        if (self != nullptr)
        {
            static_cast<DataFilesWatcher*>(self)->__CheckForChangesOnCallingThread();
        }
    }


    DataFilesWatcher::DataFilesWatcher()
        : m_root(), thread(),
          eventHandlers(), events(),
          isActive(true)
    {
    }

    DataFilesWatcher::~DataFilesWatcher()
    {
    }

    void DataFilesWatcher::AddRootDirectory(const char* directory)
    {
        easyvfs_file_info fi;
        if (easyvfs_get_file_info(g_EngineContext->GetVFS(), directory, &fi)) {
            m_root.InsertChild(fi);
        }
    }

    void DataFilesWatcher::RemoveRootDirectory(const char* directory)
    {
        m_root.RemoveChild(directory);
    }


    void DataFilesWatcher::AddEventHandler(EventHandler &eventHandler, bool postInsertEventsForExistingFiles)
    {
        if (!this->eventHandlers.Exists(&eventHandler))
        {
            this->eventHandlers.Append(&eventHandler);

            if (postInsertEventsForExistingFiles)
            {
                this->__PostOnInsertRecursively(m_root, eventHandler);
            }
        }
    }

    void DataFilesWatcher::RemoveEventHandler(EventHandler &eventHandler)
    {
        this->eventHandlers.Remove(this->eventHandlers.Find(&eventHandler));
    }


    bool DataFilesWatcher::CheckForChanges(bool asynchronous)
    {
        if (!this->EventsReady())
        {
            return false;
        }

        if (asynchronous)
        {
            this->thread.Start(DataFilesWatcher_CheckForChanges_Async, this, true);
        }
        else
        {
            this->__CheckForChangesOnCallingThread();
        }

        return true;
    }

    bool DataFilesWatcher::DispatchEvents(bool wait)
    {
        if (wait)
        {
            this->WaitForEvents();
        }
        else
        {
            if (this->thread.Busy())
            {
                return false;
            }
        }


        while (this->events.root != nullptr)
        {
            auto e = this->events.root->value;
            assert(e.item != nullptr);

            for (auto iEventHandler = this->eventHandlers.root; iEventHandler != nullptr; iEventHandler = iEventHandler->next)
            {
                auto handler = iEventHandler->value;
                assert(handler != nullptr);

                switch (e.type)
                {
                case 0: handler->OnInsert(*e.item); break;
                case 1: handler->OnRemove(*e.item); break;
                case 2: handler->OnUpdate(*e.item); break;

                default: break;
                }
            }

            // If the item in the event is a removal, the item need to be deleted.
            if (e.type == 1)
            {
                delete e.item;
            }

            this->events.RemoveRoot();
        }

        return true;
    }

    void DataFilesWatcher::CheckForChangesAndDispatchEvents()
    {
        this->CheckForChanges(false);
        this->DispatchEvents();
    }

    bool DataFilesWatcher::EventsReady()
    {
        return !this->thread.Busy();
    }

    void DataFilesWatcher::WaitForEvents()
    {
        this->thread.Wait();
    }


    void DataFilesWatcher::__CheckForChangesOnCallingThread()
    {
        // We need to recursively check directories.
        this->__CheckForChangesOnCallingThread(m_root);
    }

    void DataFilesWatcher::__CheckForChangesOnCallingThread(Item &root)
    {
        if (this->isActive)
        {
            // We first check ourselves for changes. 'root' will be the old info.
            easyvfs_file_info newInfo;
            newInfo.absolutePath[0] = '\0';
            newInfo.attributes = 0;
            newInfo.lastModifiedTime = 0;
            newInfo.sizeInBytes = 0;

            // The absolute root element will not have a valid path.
            if (root.parent != nullptr)
            {
                easyvfs_get_file_info(g_EngineContext->GetVFS(), root.info.absolutePath, &newInfo);

                if (newInfo.lastModifiedTime != root.info.lastModifiedTime)
                {
                    Event e;
                    e.type = 2;                 // <-- Update
                    e.item = &root;
                    this->events.Append(e);
                }
            }
            

        
            // Now we need to check the children for modifications. We're going to build a new map of children and then
            // compare that to the old one. We will do something special for the root item where we won't actually check
            // the file system, but instead trick it into thinking it has done so.

            GTLib::List<GTLib::String> currentChildren;

            if (&m_root != &root)
            {
                if ((newInfo.attributes & EASYVFS_FILE_ATTRIBUTE_DIRECTORY) != 0)
                {
                    easyvfs_iterator iFile;
                    if (easyvfs_begin_iteration(g_EngineContext->GetVFS(), root.info.absolutePath, &iFile))
                    {
                        easyvfs_file_info fi;
                        while (easyvfs_next_iteration(g_EngineContext->GetVFS(), &iFile, &fi))
                        {
                            currentChildren.Append(fi.absolutePath);
                        }
                    }
                }
            }
            else
            {
                for (size_t i = 0; i < root.children.count; ++i)
                {
                    currentChildren.Append(root.children.buffer[i]->value->info.absolutePath);
                }
            }


            // We now have our list of children. We need to cross reference the new list of children against the list
            // currently in memory and post the appropriate events. We'll start with the children that have been removed.
            GTLib::List<Item*> removedChildren;
            for (size_t i = 0; i < root.children.count; ++i)
            {
                auto  iItem = root.children.buffer[i]->value;
                auto &iPath = iItem->info.absolutePath;

                if (currentChildren.Find(iPath) == nullptr)
                {
                    removedChildren.Append(iItem);

                    Event e;
                    e.type = 1;                 // <-- Remove
                    e.item = iItem;
                    this->events.Append(e);
                }
            }

            // Now we find the new files.
            GTLib::List<Item*> addedChildren;
            for (auto iChild = currentChildren.root; iChild != nullptr; iChild = iChild->next)
            {
                easyvfs_file_info info;
                easyvfs_get_file_info(g_EngineContext->GetVFS(), iChild->value.c_str(), &info);

                if (root.children.Find(info.absolutePath) == nullptr)
                {
                    auto newItem = new Item(info, &root);
                    addedChildren.Append(newItem);

                    Event e;
                    e.type = 0;                 // <-- Insert
                    e.item = newItem;
                    this->events.Append(e);
                }
            }




            // The root info needs to be updated if it's actually changed.
            if (newInfo.lastModifiedTime != root.info.lastModifiedTime)
            {
                root.info = newInfo;
            }
        
            // We need to actually remove the children from root's children list.
            for (auto iChild = removedChildren.root; iChild != nullptr; iChild = iChild->next)
            {
                root.children.Remove(iChild->value->info.absolutePath);
            }

            // And now the new children.
            for (auto iChild = addedChildren.root; iChild != nullptr; iChild = iChild->next)
            {
                root.children.Add(iChild->value->info.absolutePath, iChild->value);
            }



            // Now we need to check the children.
            for (size_t i = 0; i < root.children.count; ++i)
            {
                this->__CheckForChangesOnCallingThread(*root.children.buffer[i]->value);
            }
        }
    }

    void DataFilesWatcher::__Deactivate()
    {
        this->isActive = false;
    }



    void DataFilesWatcher::__PostOnInsertRecursively(const Item &rootItem, EventHandler &eventHandler)
    {
        // We want to ignore root items here.
        if (&rootItem != &m_root && rootItem.parent != &m_root)
        {
            eventHandler.OnInsert(rootItem);
        }


        for (size_t i = 0; i < rootItem.children.count; ++i)
        {
            auto iChild = rootItem.children.buffer[i];
            assert(iChild        != nullptr);
            assert(iChild->value != nullptr);

            this->__PostOnInsertRecursively(*iChild->value, eventHandler);
        }
    }
}

