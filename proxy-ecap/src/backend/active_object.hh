//
// Created by igor on 01/09/2020.
//

#ifndef BINADOX_ECAP_ACTIVE_OBJECT_HH
#define BINADOX_ECAP_ACTIVE_OBJECT_HH

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <memory>

namespace binadox
{
    template <typename T>
    class shared_queue
    {
    public:
        shared_queue() { };
        virtual ~shared_queue() { };
        void push(const T& pushed_item)
        {
            std::lock_guard<std::mutex> guard(mutex);
            queue.push(pushed_item);
            cond.notify_one();
        }

        bool empty() const
        {
            return queue.empty();
        }

        std::size_t size() const
        {
            return queue.size();
        }

        void pop(T& popped_item)
        {
            std::unique_lock<std::mutex> guard(mutex);
            while (queue.empty())
            {
                cond.wait(guard);
            }
            popped_item = queue.front();
            queue.pop();
        }

    private:
        std::queue<T> queue;
        mutable std::mutex mutex;
        std::condition_variable cond;

        shared_queue(const shared_queue&);
        shared_queue& operator=(const shared_queue&);
    };



    class active_object
    {
    public:
        typedef std::function<void() > job_t;
    public:
        static std::unique_ptr<active_object> create();
        ~active_object();
        void post(job_t job_to_schedule);
    private:
        void stop();
        void run();

        shared_queue<job_t> job_queue;
        std::thread thread;
        bool continue_work;

        active_object();

        active_object(const active_object&);
        void operator=(const active_object&);
    };

}

#endif //BINADOX_ECAP_ACTIVE_OBJECT_HH
