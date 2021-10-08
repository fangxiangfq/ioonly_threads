#pragma once

#include <functional>
#include <memory>
namespace Event
{
    class EventsLoop;
    class Event :public std::enable_shared_from_this<Event> 
    {
    public:
        using EventCallback = std::function<void()>;

        Event(EventsLoop& loop, int fd);
        ~Event();
        int fd() const { return fd_; }
        int events() const { return events_; }
        void enableRead() { events_ |= kReadEvent; update(); }
        void disableRead() { events_ &= ~kReadEvent; update(); }
        void enableWrite() { events_ |= kWriteEvent; update(); }
        void disableWrite() { events_ &= ~kWriteEvent; update(); }
        void disableAll() { events_ = kNoneEvent; update(); }
        bool isWriting() const { return events_ & kWriteEvent; }
        bool isReading() const { return events_ & kReadEvent; }
        void setRead(const EventCallback& cb) {readCallback_ = cb};
        void setWrite(const EventCallback& cb) {writeCallback_ = cb};
    private:
        void update();
        EventsLoop& loop_;
        const int fd_;
        int  events_;

        EventCallback readCallback_;
        EventCallback writeCallback_;
        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;
    };

    using EventPtr = std::shared_ptr<Event>;
} // namespace Event
  
 