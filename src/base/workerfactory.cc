#include "workerfactory.h"
#include <assert.h>
using std::placeholders::_1;

namespace Thread
{
    WorkerFactory::WorkerFactory(Event::EventsLoop* baseLoop, const std::string& nameArg, uint16_t threadNum)
    :baseLoop_(baseLoop),
    name_(nameArg),
    started_(false),
    threadNum_(threadNum),
    workerIdx_(0),
    threads_(threadNum_)
    {
        init();
    }
    
    WorkerFactory::~WorkerFactory() 
    {
        // Don't delete loop, it's stack variable
    }
    
    void WorkerFactory::init() 
    {
        assert(!started_);
        for (int i = 0; i < threadNum_; ++i) 
        { 
            workertaskEv_.emplace_back(EvManager::createTaskEvPtr());
            workertaskEv_[i]->setTaskCb(std::bind(WorkerFactory::execTask, _1));
            mastertaskEv_.emplace_back(EvManager::createTaskEvPtr());
        }
    }
    
    void WorkerFactory::start(const WorkInitCb& cb) 
    {
        assert(!started_);
        baseLoop_->assertSelfThread();

        started_ = true;

        for (int i = 0; i < threadNum_; ++i)
        {
            char buf[name_.size() + 32];
            snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);
            std::unique_ptr<Thread> worker(new Thread(workertaskEv_[i], cb, std::string(buf)));
            loops_.push_back(worker->startLoop());
            threads_.push_back(std::move(worker));
        }

        if (0 == threadNum_ && cb)
        {
            cb(baseLoop_);
        }
    }
    
    uint16_t WorkerFactory::getNextIdx() 
    {
        ++workerIdx_;
        if (workerIdx_ >= loops_.size())
        {
            workerIdx_ = 0;
        }
        return workerIdx_;
    }

    Event::EventsLoop* WorkerFactory::getNextLoop() 
    {
        baseLoop_->assertSelfThread();
        assert(started_);
        Event::EventsLoop* loop = baseLoop_;

        if(!loops_.empty())
        {
            loop = loops_[workerIdx_];
            ++workerIdx_;
            if (workerIdx_ >= loops_.size())
            {
                workerIdx_ = 0;
            }
        }

        return loop;
    }
    
    Event::EventsLoop* WorkerFactory::getLoopForHash(size_t hashCode) 
    {
        baseLoop_->assertSelfThread();
        Event::EventsLoop* loop = baseLoop_;

        if (!loops_.empty())
        {
            loop = loops_[hashCode % loops_.size()];
        }
        return loop;
    }

    void WorkerFactory::postTask(TaskCb cb) 
    {
        if(threadNum_ == 0){
            cb();
            return;
        }

        TaskData* task = new TaskData(std::move(cb));
        uint16_t idx = getNextIdx();

        auto& workerev = workertaskEv_[idx];

        workerev->write(task);
    }

    void WorkerFactory::execTask(void* taskdata) 
    {
        TaskData* task = static_cast<TaskData*>(taskdata);
        if(task->cb_)
            task->cb_();
        
        delete task;
    }
}