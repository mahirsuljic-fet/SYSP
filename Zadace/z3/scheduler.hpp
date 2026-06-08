#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <format>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

#include "color.hpp"
#include "task.hpp"

class Scheduler
{
  public:
    Scheduler(std::size_t thread_count = std::thread::hardware_concurrency());
    Scheduler(Scheduler&&) = delete;
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(Scheduler&&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;
    ~Scheduler();

    void run();
    void cancel(Task::ID id);

    void wait_for_all()
    {
      std::unique_lock<std::mutex> lck { mtx };
      cv.wait(lck, [this] { return tasks_.empty() && active_threads_ == 0; });
    }

    template <typename T>
    void add(T&& task)
    {
      std::lock_guard<std::mutex> lg { mtx };
      auto add_msg = std::format("{}Adding task {}{}\n", CYAN, task.id(), RESET);
      std::cout << add_msg;
      tasks_.push(std::forward<T>(task));
      cv.notify_one();
    }

  private:
    struct TaskComparator
    {
        bool operator()(const Task& lhs, const Task& rhs)
        {
          if (lhs.start_time() != rhs.start_time())
            return lhs.start_time() > rhs.start_time();
          else
            return lhs.priority() > rhs.priority();
        }
    };

    void show_statistics();

    std::priority_queue<Task, std::vector<Task>, TaskComparator> tasks_;
    std::unordered_set<Task::ID> canceled_task_ids_;
    std::vector<std::jthread> threads_;
    std::atomic<bool> running_;

    std::condition_variable cv;
    std::mutex mtx;

    std::atomic<std::size_t> active_threads_;
    std::atomic<std::size_t> executed_tasks_;
    std::atomic<double> task_duration_sum_;
};
