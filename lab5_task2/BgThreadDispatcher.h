//
// Created by Gregogo on 07.11.2024.
//

#ifndef LAB5_TASK2_BGTHREADDISPATCHER_H
#define LAB5_TASK2_BGTHREADDISPATCHER_H

#include <iostream>

#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <atomic>
#include <vector>
#include <iostream>
#include <syncstream>

class BgThreadDispatcher
{
public:
    using Task = std::function<void()>;

    BgThreadDispatcher() :
            m_taskRunning(false),
            m_stopFlag(false),
            m_thread(&BgThreadDispatcher::ThreadLoop, this)
    {}

    ~BgThreadDispatcher()
    {
        Stop();
    }

    void Dispatch(Task task)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_stopFlag)
        {
            m_tasks.push(std::move(task));
            //возмоен дедлок
            lock.unlock();
            m_tasksChangedOrStopped.notify_one();
        }
    }

    void Wait()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_allTasksDone.wait(lock, [this]
        {
            return m_tasks.empty() && !m_taskRunning;
        });
    }

    void Stop()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stopFlag = true;
        m_tasksChangedOrStopped.notify_one();
    }

private:
    std::queue<Task> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_tasksChangedOrStopped;
    std::condition_variable m_allTasksDone;
    std::atomic<bool> m_stopFlag;
    std::atomic<bool> m_taskRunning;
    std::jthread m_thread;


    void ThreadLoop()
    {
        while (true)
        {
            Task task;
            {
                //можно ли перенести работу с флагом ближе сюда, чтобы было меньше локов
                std::unique_lock<std::mutex> lock(m_mutex);
                if (m_tasks.empty())
                {
                    m_tasksChangedOrStopped.wait(lock, [this]
                    {
                        return m_stopFlag || !m_tasks.empty();
                    });
                }

                if (m_stopFlag && m_tasks.empty())
                    break;

                task = std::move(m_tasks.front());
                m_tasks.pop();

                m_taskRunning = true;
            }

            try
            {
                task();
            } catch (...)
            {
                // Игнорируем :)))
            }

            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_taskRunning = false;
                if (m_tasks.empty())
                {
                    lock.unlock();
                    m_allTasksDone.notify_all();
                }
                //возможен дедлок
            }
        }
    }
};

#endif //LAB5_TASK2_BGTHREADDISPATCHER_H
