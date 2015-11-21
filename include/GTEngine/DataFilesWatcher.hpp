// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_DataFilesWatcher_hpp_
#define __GTEngine_DataFilesWatcher_hpp_

#include <GTLib/IO.hpp>
#include <GTLib/Dictionary.hpp>
#include <GTLib/Threading.hpp>

namespace GTEngine
{
    /// Class for watching the data directories for changes, and then post events for any changes.
    ///
    /// This class is designed to be run on multiple threads. To check for changes, call the asynchronous method CheckForChanges(). This method
    /// will cache the changes which can be dispatched with DispatchEvents(). Use EventsReady() to determine if the events are ready, or wait
    /// for the events with WaitForEvents().
    ///
    /// There is one instantiation of this class for every Game object.
    class DataFilesWatcher
    {
    public:

        ///////////////////////////////////////////////////////////
        // Data structures.

        /// Structure representing a file or directory.
        struct Item
        {
            Item()
                : info(),
                  relativePath(), absolutePath(),
                  parent(nullptr), children()
            {
            }

            Item(const GTLib::FileInfo &infoIn, Item* parent)
                : info(infoIn),
                  relativePath(infoIn.path), absolutePath(infoIn.absolutePath),
                  parent(parent), children()
            {
            }

            ~Item()
            {
                for (size_t i = 0; i < this->children.count; ++i)
                {
                    delete this->children.buffer[i]->value;
                }
            }

            
            /// Adds a child item.
            ///
            /// @param info [in] The file info of the child.
            void InsertChild(const GTLib::FileInfo &infoIn)
            {
                this->children.Add(infoIn.path.c_str(), new Item(infoIn, this));
            }

            /// Removes a child item.
            ///
            /// @param info [in] The child being removed.
            void RemoveChild(Item* item)
            {
                if (item != nullptr)
                {
                    this->RemoveChild(item->info.path.c_str());
                }

                delete item;
            }

            /// Removes a child item by it's name.
            ///
            /// @param name [in] The name of the child being removed.
            void RemoveChild(const char* name)
            {
                auto iItem = this->children.Find(name);
                if (iItem != nullptr)
                {
                    auto item = iItem->value;

                    this->children.RemoveByIndex(iItem->index);

                    delete item;
                }
            }

            /// Retrieves the absolute directory of the top level root item.
            const GTLib::String & GetRootDirectory()
            {
                if (this->parent != nullptr && this->parent->parent == nullptr)
                {
                    return this->info.absolutePath;
                }
                else
                {
                    return this->parent->GetRootDirectory();
                }
            }


            /// The file info.
            GTLib::FileInfo info;

            /// The relative path.
            GTLib::String relativePath;

            /// The absolute path.
            GTLib::String absolutePath;


            /// A pointer to the parent item. Can be null.
            Item* parent;

            /// The list of children. We use a dictionary here in order to keep everything in alphabetical order and to also allow fast searching.
            GTLib::Dictionary<Item*> children;
            
            
        private:    // No copying.
            Item(const Item &);
            Item & operator=(const Item &);
        };

        /// Base class for handling events.
        class EventHandler
        {
        public:
            
            /// Explicit virtual destructor for silencing a GCC warning.
            virtual ~EventHandler() {};

            /// Called when a file is added/inserted.
            ///
            /// @param item [in] A reference to the item that was inserted.
            virtual void OnInsert(const Item &) {};

            /// Called when a file is removed.
            ///
            /// @param item [in] A reference to the item that was removed.
            ///
            /// @remarks
            ///     The attributes in 'item' will be set to those before the removal. Thus, the parent will be still set to the old parent, and not the new parent.
            virtual void OnRemove(const Item &) {};

            /// Called when a file is updated.
            ///
            /// @param item [in] A reference to the item that was updated.
            virtual void OnUpdate(const Item &) {};
        };




    public:

        /// Constructor.
        DataFilesWatcher();

        /// Destructor.
        ~DataFilesWatcher();


        /// Adds a root directory to use for checking for changes.
        ///
        /// @param directory [in] The directory to add.
        ///
        /// @remarks
        ///     Calling this event will cause Insert events to be called.
        void AddRootDirectory(const char* directory);

        /// Removes a root directory.
        ///
        /// @param directory [in] The directory to remove.
        void RemoveRootDirectory(const char* directory);


        /// Adds an event handler.
        ///
        /// @param eventHandler     [in] A reference to the event handler to add.
        /// @param postInsertEvents [in] If set to true, posts OnInsert events for already-added files. Defaults to true.
        void AddEventHandler(EventHandler &eventHandler, bool postInsertEventsForExistingFiles = true);

        /// Removes an event handler.
        ///
        /// @param eventHandler [in] A reference to the event handler to remove.
        void RemoveEventHandler(EventHandler &eventHandler);




        /// Performs a full recursive check for any changes.
        ///
        /// @param asynchronous [in] Whether or not the operation should be performed asynchronously (defaults to true).
        ///
        /// @return True if the events have started being checked; false otherwise.
        ///
        /// @remarks
        ///     If the watcher is in the middle of performing a check, this will return false.
        bool CheckForChanges(bool asynchronous = true);

        /// Dispatches any events founds by CheckForChanges().
        ///
        /// @param wait [in] Whether or not the method should wait for changes to complete.
        ///
        /// @remarks
        ///     This will return false if changes are in the process of being checked and 'wait' is set to false.
        bool DispatchEvents(bool wait = true);

        /// Synchronously calls CheckForChanges() and DispatchEvents().
        void CheckForChangesAndDispatchEvents();

        /// Determines whether or not the events from the previous call to CheckForChanges() are ready.
        bool EventsReady();

        /// Waits for CheckForChanges() to finish.
        ///
        /// @remarks
        ///     You can wait for, and dispatch events at once by doing DispatchEvents(true).
        void WaitForEvents();



        /// Performs the actual checks on the calling thread. Do not call this directly.
        void __CheckForChangesOnCallingThread();
        void __CheckForChangesOnCallingThread(Item &root);


        /// Deactives the watcher for faster termination at shutdown.
        ///
        /// @remarks
        ///     This method should only be used internally.
        void __Deactivate();




    ///////////////////////////////////////////////////
    // Private Methods

    private:

        /// Recursively posts OnInsert events to the given event handler.
        ///
        /// @param rootItem     [in] The root item to post.
        /// @param eventHandler [in] The event handler to posts the events to.
        void __PostOnInsertRecursively(const Item &rootItem, EventHandler &eventHandler);



    ///////////////////////////////////////////////////
    // Attributes

    private:

        /// The root item. This children of this item will be the root directories specified by AddRootDirectory().
        Item m_root;

        /// The thread that will perform the asynchronous file checks.
        GTLib::Thread thread;


        /// The list of event handlers currently attached.
        GTLib::List<EventHandler*> eventHandlers;


        /// Structure representing an event.
        struct Event
        {
            int type;           // 0 = Insert, 1 = Remove, 2 = Update.
            Item* item;
        };

        /// The list of events waiting to be dispatched.
        GTLib::List<Event> events;


        /// This keeps track of whether or not the data files watcher is still active.
        bool isActive;
    };
}

#endif
