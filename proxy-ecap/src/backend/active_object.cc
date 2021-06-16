//
// Created by igor on 01/09/2020.
//

#include "backend/active_object.hh"

namespace binadox
{
    active_object::active_object()
            : continue_work(true)
    {

    }
    // --------------------------------------------------------------------------------------
    std::unique_ptr<active_object> active_object::create()
    {
        std::unique_ptr<active_object> new_ptr(new active_object());
        new_ptr->thread = std::thread(&active_object::run, new_ptr.get());
        return new_ptr;
    }
    // --------------------------------------------------------------------------------------
    active_object::~active_object()
    {
        job_t stop = std::bind(&active_object::stop, this);
        post(stop);
        if (thread.joinable())
        {
            thread.join();
        }
    }
    // --------------------------------------------------------------------------------------
    void active_object::post(job_t job_to_schedule)
    {
        job_queue.push(job_to_schedule);
    }
    // --------------------------------------------------------------------------------------
    void active_object::stop()
    {
        continue_work = false;
    }
    // --------------------------------------------------------------------------------------
    void active_object::run()
    {
        while (continue_work)
        {
            job_t job_to_execute;
            job_queue.pop(job_to_execute);
            job_to_execute();
        }
    }
} // ns binadox

