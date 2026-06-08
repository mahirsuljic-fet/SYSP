#include "scheduler.hpp"
#include "task.hpp"

int main()
{
  Scheduler sched { 5 };

  sched.add(Task { Task::Delay { 1000 }, Task::Duration { 3000 }, Task::Priority::MEDIUM }); // ID 1
  sched.add(Task { Task::Delay { 1000 }, Task::Duration { 1000 }, Task::Priority::HIGH });   // ID 2
  sched.add(Task { Task::Delay { 1500 }, Task::Duration { 2000 }, Task::Priority::LOW });    // ID 3
  sched.add(Task { Task::Delay { 1500 }, Task::Duration { 1000 }, Task::Priority::HIGH });   // ID 4

  auto task_to_cancel = Task { Task::Delay { 5000 }, Task::Duration { 1000 }, Task::Priority::LOW }; // ID 5
  Task::ID id_to_cancel = task_to_cancel.id();
  sched.add(std::move(task_to_cancel));
  sched.cancel(id_to_cancel);

  sched.wait_for_all();

  return 0;
}
