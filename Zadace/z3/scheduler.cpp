#include <chrono>
#include <cstddef>
#include <format>
#include <mutex>
#include <queue>
#include <sstream>
#include <vector>

#include "color.hpp"
#include "scheduler.hpp"

Scheduler::Scheduler(std::size_t thread_count)
  : tasks_(),
    canceled_task_ids_(),
    threads_(),
    running_(false),
    active_threads_ { 0ul },
    executed_tasks_ { 0ul },
    task_duration_sum_ { 0.0 }
{
  threads_.reserve(thread_count);
  for (auto i = 0ul; i < thread_count; ++i)
  {
    auto start_msg = std::format("{}Starting thread {}{}\n", BLUE, i, RESET);
    std::cout << start_msg;
    threads_.emplace_back([this] { run(); });
  }
  std::lock_guard<std::mutex> lck { mtx };
  running_ = true;
  cv.notify_all();
}

void Scheduler::run()
{
  {
    std::unique_lock<std::mutex> lck { mtx };
    cv.wait(lck, [this] { return running_.load(); });
  }
  while (running_)
  {
    std::unique_lock<std::mutex> lck { mtx };
    cv.wait(lck, [this] { return !tasks_.empty() || !running_; });
    if (!running_) break;

    // Check if the current task is canceled
    auto canceled_id_it = canceled_task_ids_.find(tasks_.top().id());
    bool is_canceled = canceled_id_it != canceled_task_ids_.cend();
    if (is_canceled)
    {
      tasks_.pop();
      canceled_task_ids_.erase(canceled_id_it);
      continue;
    }

    // Wait for the next task
    auto next_start_time = tasks_.top().start_time();
    cv.wait_until(lck, next_start_time, [this, next_start_time] {
      if (!running_) return true;
      if (tasks_.empty()) return false;
      bool earlier_task_available = tasks_.top().start_time() < next_start_time;
      bool task_canceled = canceled_task_ids_.find(tasks_.top().id()) != canceled_task_ids_.cend();
      return earlier_task_available || task_canceled;
    });
    if (!running_) break;

    if (!tasks_.empty()
      && !(canceled_task_ids_.find(tasks_.top().id()) != canceled_task_ids_.cend())
      && std::chrono::system_clock::now() >= tasks_.top().start_time())
    {
      Task current_task = tasks_.top();
      tasks_.pop();
      show_statistics(); // before running the task
      active_threads_++;
      lck.unlock();

      current_task();

      lck.lock();
      active_threads_--;
      executed_tasks_++;
      task_duration_sum_ += current_task.duration().count();
      show_statistics(); // after running the task
      lck.unlock();

      cv.notify_all();
    }
  }
}

void Scheduler::cancel(Task::ID id)
{
  std::lock_guard<std::mutex> lck { mtx };
  auto cancel_msg = std::format("{}Canceling task {}{}\n", RED, id, RESET);
  std::cout << cancel_msg;
  canceled_task_ids_.emplace(id);
  cv.notify_all();
}

Scheduler::~Scheduler()
{
  std::lock_guard<std::mutex> lck { mtx };
  running_ = false;
  cv.notify_all();
}

void Scheduler::show_statistics()
{
  std::stringstream ss;
  ss << "==================== SYSTEM ====================\n";
  ss << std::format("-> Active threads: {}/{}\n", active_threads_.load(), threads_.size());
  ss << std::format("-> Executed tasks: {}\n", executed_tasks_.load());
  ss << std::format("-> Average task duration: {:.2f}ms\n", (executed_tasks_ > 0) ? task_duration_sum_ / executed_tasks_ : 0);
  ss << "================================================\n";
  std::cout << ss.str();
}
