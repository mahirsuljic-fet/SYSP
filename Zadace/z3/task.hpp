#pragma once

#include <atomic>
#include <chrono>
#include <cstddef>
#include <functional>

class Task
{
  public:
    enum class Priority
    {
      HIGH = 0ul,
      MEDIUM = 1ul,
      LOW = 2ul
    };

    using Timestamp = std::chrono::time_point<std::chrono::system_clock>;
    using Duration = std::chrono::milliseconds;
    using Delay = std::chrono::milliseconds;
    using ID = std::size_t;

    Task(std::function<void(void)>&& task, Delay delay, Duration duration, Priority priority = Priority::LOW);
    Task(Delay delay, Duration duration, Priority priority = Priority::LOW);

    void operator()() const { task_(); }

    Timestamp start_time() const { return start_time_; }
    Duration duration() const { return duration_; }
    Priority priority() const { return priority_; }
    ID id() const { return id_; }

  private:
    std::function<void(void)> task_;
    Timestamp start_time_;
    Duration duration_;
    Priority priority_;
    ID id_;

    static std::atomic<ID> next_id;

    static void default_task(ID id, Duration duration);
};
