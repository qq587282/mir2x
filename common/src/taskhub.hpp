/*
 * =====================================================================================
 *
 *       Filename: taskhub.hpp
 *        Created: 04/03/2016 22:14:46
 *  Last Modified: 05/24/2016 00:23:16
 *
 *    Description: this makes me very confused, std::function may use internally
 *                 dynamically allocated memory, if so, it's nonsense of using
 *                 the MemoryBlockPN to allocated std::function itself
 *
 *                 OK do as much as you can
 *
 *                 I am not confident enough to update this class
 *
 *
 *        Version: 1.0
 *       Revision: none
 *       Compiler: gcc
 *
 *         Author: ANHONG
 *          Email: anhonghe@gmail.com
 *   Organization: USTC
 *
 * =====================================================================================
 */
#pragma once

#include <list>
#include <condition_variable>

#include "task.hpp"
#include "basehub.hpp"
#include "memoryblockpn.hpp"

using TaskBlockPN = MemoryBlockPN<sizeof(Task), 1024, 4>;
class TaskHub: public BaseHub<TaskHub>
{
    protected:
        std::mutex              m_TaskLock;
        std::condition_variable m_TaskCV;
        std::list<Task*>        m_TaskList;
        TaskBlockPN             m_TaskBlockPN;

    public:
        TaskHub()
            : BaseHub<TaskHub>()
        {}
        virtual ~TaskHub() = default;
        
    protected:
        void Add(Task* pTask, bool bPushHead = false)
        {
            if(!pTask){ return; }

            bool bDoSignal = false;
            {
                std::lock_guard<std::mutex> stLockGuard(m_TaskLock);
                // still we are running
                if(State() == 2){
                    bDoSignal = m_TaskList.empty();

                    if(bPushHead){
                        m_TaskList.push_front(pTask);
                    }else{
                        m_TaskList.push_back(pTask);
                    }
                }else{
                    DeleteTask(pTask);
                    return;
                }
            }

            if(bDoSignal){
                m_TaskCV.notify_one();
            }
        }

    public:
        template<typename... Args> void Add(Args &&... args, bool bPushHead = false)
        {
            Add(CreateTask(std::forward<Args>(args)...), bPushHead);
        }

    public:
        template<typename... Args> Task *CreateTask(Args &&... args)
        {
            void *pData = m_TaskBlockPN.Get();

            // passing null argument to placement new is undefined behavior
            if(!pData){ return nullptr; }
            return new (pData) Task(std::forward<Args>(args)...);
        }

        void DeleteTask(Task *pTask)
        {
            pTask->~Task();
            m_TaskBlockPN->Free(pTask);
        }

    public:
        void MainLoop()
        {
            // NOTE: second argument defer_lock is to prevent from immediate locking
            std::unique_lock<std::mutex> stTaskUniqueLock(m_TaskLock, std::defer_lock);

            while(State() != 0){
                // check if there are tasks waiting
                stTaskUniqueLock.lock();

                if(m_TaskList.empty()){
                    //if the list is empty, then wait for signal
                    m_TaskCV.wait(stTaskUniqueLock);
                }

                if(!m_TaskList.empty()){
                    auto pTask = m_TaskList.front();
                    m_TaskList.pop_front();
                    stTaskUniqueLock.unlock();

                    if(!pTask->Expired()){ (*pTask)(); }

                    DeleteTask(pTask);
                }else{
                    stTaskUniqueLock.unlock();
                }
            }
        }

        void Shutdown()
        {
            // TODO
            // we may need to clear the task queue
            // otherwise we'll run into trouble if we do
            //
            //      pTaskHub = new TaskHub();
            //      pTaskHub->Shutdown();
            //      pTaskHub->Launch();
            // think about how to implement it by either
            //  1. implement an task clean function
            //  2. let Shutdown() block until all task finished
            auto pTask = CreateTask([this](){ State(0); m_TaskCV.notify_one(); });

            std::lock_guard<std::mutex> stLockGuard(m_TaskLock);
            m_TaskList.push_back(pTask);
            m_TaskCV.notify_one();
        }
};
