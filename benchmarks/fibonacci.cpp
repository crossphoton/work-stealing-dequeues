#include "../WSQ.h"
#include "../task.h"

#include <cstdint>
#include <math.h>
#include <random>
#include <stdio.h>
#include <vector>

#define FIB_LIMIT 10

uint64_t TASK_ID = 0;

struct fib_task {
  uint64_t id;
  int n;
  uint64_t result;
  fib_task(uint64_t _id, int _n) : id(_id), n(_n) {}
  fib_task(uint64_t _id, int _n, uint64_t _result)
      : id(_id), n(_n), result(_result) {}
};

std::vector<fib_task> tasks;

uint64_t fib(int n) {
  if (n <= 1)
    return n;
  return fib(n - 1) + fib(n - 2);
}

void lambda(fib_task *task) {
  // fprintf(stdout, "working for %d\n", task->n);
  uint64_t result = fib(task->n);
  *task = fib_task(task->id, task->n, result);
  usleep(1000);
  // fprintf(stdout, "completed %d  -> %ld\n", task->n, result);
}

wsq::Runnable runnable_generator() {
  int n = 3 + (rand() % FIB_LIMIT);
  fib_task task(++TASK_ID, n);
  tasks.push_back(task);

  return wsq::Runnable(lambda, &tasks[tasks.size() - 1]);
}

void collect_results(long duration, long start, long end) {
  fprintf(stdout, "total tasks done: %ld in %ldms\nid\t\t\tindex\t\t\tresult\n",
          tasks.size(), duration);
  for (auto &a : tasks)
    fprintf(stdout, "%ld\t\t\t%d\t\t\t%ld\n", a.id, a.n, a.result);
}