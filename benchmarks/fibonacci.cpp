#include "../WSQ.h"
#include "../task.h"

#include <cstdint>
#include <math.h>
#include <random>
#include <vector>

#define FIB_LIMIT 5

uint64_t TASK_ID = 0;

struct fib_task {
  uint64_t id;
  int n;
  uint64_t result;
  fib_task(uint64_t _id, int _n) : id(_id), n(_n) {}
};

std::vector<fib_task> tasks;

uint64_t fib(long _index) {
  if (_index < 1)
    return _index;
  return fib(_index - 1) + fib(_index - 2);
}

void lambda(fib_task *task) {
  uint64_t result = fib(task->n);
  task->result = result;
}

wsq::Runnable runnable_generator() {
  srand(time(0));
  int n = rand() % FIB_LIMIT;
  fib_task task(++TASK_ID, n);
  tasks.push_back(task);

  return wsq::Runnable(lambda, &tasks[tasks.size() - 1]);
}

void collect_results() {}