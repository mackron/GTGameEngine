// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Core/Threading/JobQueue.hpp>

namespace GT
{
    JobQueue::JobQueue() 
        : frontRoot(nullptr), frontEnd(nullptr), backRoot(nullptr), backEnd(nullptr)
    {
    }
        
    JobQueue::~JobQueue()
    {
    }
        
    void JobQueue::Push(Job *job)
    {
        if (this->backEnd != nullptr)
        {
            this->backEnd->next = job;
            this->backEnd = backEnd->next;
        }
        else
        {
            this->backRoot = job; // If the end item was nullptr, so was the root.
            this->backEnd  = job;
        }
    }
        
    Job * JobQueue::Pop()
    {
        if (this->frontRoot != nullptr)
        {
            this->frontRoot->Run();
                
            Job *next = this->frontRoot->next;
            delete this->frontRoot;
            this->frontRoot = next;
                
            // If the root is nullptr, so is the end.
            if (this->frontRoot == nullptr)
            {
                this->frontEnd = nullptr;
            }
        }
            
        return this->frontRoot;
    }
        
    void JobQueue::PumpJobs()
    {
        if (this->frontEnd != nullptr)
        {
            this->frontEnd->next = this->backRoot;
        }
        else // frontRoot will be nullptr when frontEnd is also nullptr.
        {
            this->frontRoot = this->backRoot;
        }
        this->frontEnd = this->backEnd;
            
        // After appending, the back jobs will be null again.
        this->backRoot = nullptr;
        this->backEnd  = nullptr;
    }
}
