
#include <GTEngine/DataFilesWatcher.hpp>

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
        : root(), thread(),
          eventHandlers(), events(),
          isActive(true)
    {
    }

    DataFilesWatcher::~DataFilesWatcher()
    {
    }

    void DataFilesWatcher::AddRootDirectory(const char* directory)
    {
        this->root.InsertChild(GTCore::IO::FileInfo(directory));
    }

    void DataFilesWatcher::RemoveRootDirectory(const char* directory)
    {
        this->root.RemoveChild(directory);
    }


    void DataFilesWatcher::AddEventHandler(EventHandler &eventHandler, bool postInsertEventsForExistingFiles)
    {
        if (!this->eventHandlers.Exists(&eventHandler))
        {
            this->eventHandlers.Append(&eventHandler);

            if (postInsertEventsForExistingFiles)
            {
                this->__PostOnInsertRecursively(this->root, eventHandler);
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
        this->__CheckForChangesOnCallingThread(this->root);
    }

    void DataFilesWatcher::__CheckForChangesOnCallingThread(Item &root)
    {
        if (this->isActive)
        {
            // We first check ourselves for changes. 'root' will be the old info.
            GTCore::IO::FileInfo newInfo;
            GTCore::IO::GetFileInfo(root.info.absolutePath.c_str(), newInfo);

            if (newInfo.lastModifiedTime != root.info.lastModifiedTime)
            {
                Event e;
                e.type = 2;                 // <-- Update
                e.item = &root;
                this->events.Append(e);
            }

        
            // Now we need to check the children for modifications. We're going to build a new map of children and then
            // compare that to the old one. We will do something special for the root item where we won't actually check
            // the file system, but instead trick it into thinking it has done so.

            GTCore::List<GTCore::String> currentChildren;

            if (&this->root != &root)
            {
                if (newInfo.isDirectory)
                {
                    GTCore::IO::FileIterator i((root.info.absolutePath + "/.*").c_str());
                    while (i)
                    {
                        currentChildren.Append(i.name);
                        ++i;
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
            GTCore::List<Item*> removedChildren;
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
            GTCore::List<Item*> addedChildren;
            for (auto iChild = currentChildren.root; iChild != nullptr; iChild = iChild->next)
            {
                GTCore::IO::FileInfo info;
                GTCore::IO::GetFileInfo(iChild->value.c_str(), info);

                if (root.children.Find(info.absolutePath.c_str()) == nullptr)
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
                root.children.Remove(iChild->value->info.absolutePath.c_str());
            }

            // And now the new children.
            for (auto iChild = addedChildren.root; iChild != nullptr; iChild = iChild->next)
            {
                root.children.Add(iChild->value->info.absolutePath.c_str(), iChild->value);
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
        if (&rootItem != &this->root && rootItem.parent != &this->root)
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

