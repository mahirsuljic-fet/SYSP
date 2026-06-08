#include <atomic>
#include <chrono>
#include <format>
#include <functional>
#include <iostream>
#include <thread>

#include "color.hpp"
#include "task.hpp"

std::atomic<Task::ID> Task::next_id = 1;

Task::Task(std::function<void(void)>&& task, Delay delay, Duration duration, Priority priority)
  : task_ { std::move(task) },
    start_time_ { std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() + delay) }, // round to ms
    duration_ { duration },
    priority_ { priority },
    id_ { next_id++ }
{
}

Task::Task(Delay delay, Duration duration, Priority priority)
  : Task(nullptr, delay, duration, priority)
{
  task_ = std::bind(default_task, id_, duration_);
}

void Task::default_task(ID id, Duration duration)
{
  auto start_msg = std::format("{}Starting task {}{}\n", GREEN, id, RESET);
  auto end_msg = std::format("{}Ending task {}{}\n", YELLOW, id, RESET);
  std::cout << start_msg;
  std::this_thread::sleep_for(duration); // simulate work
  std::cout << end_msg;
}
