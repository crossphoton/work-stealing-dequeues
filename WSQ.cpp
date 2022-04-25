#ifndef _PCP_WS_QUEUE
#define _PCP_WS_QUEUE

#include "WSQ.h"
#include <atomic>
#include <functional>
#include <stdio.h>
#include <thread>
#include <unistd.h>

namespace wsq {
long Runnable::__runnable_id = 0;

void WorkStealingThread::run() {
#ifdef __DEBUG_PRINT
  printf("starting work steal %d\n", me);
#endif
  Runnable *task = queue[me]->popBottom();
  int count = 0;
  while (true) {
    while (task != nullptr) {
      task->run();
      task = queue[me]->popBottom();
      count++;
    }

    if (this->enableStealing) {
      while (task == nullptr) {
        std::this_thread::yield();
        int victim = 0;
        for (; victim < totalQueues; victim++) {
          if (victim == me || queue[victim]->isEmpty())
            continue;
          break;
        }
        if (victim + 1 >= totalQueues)
          return;
        if (!queue[victim]->isEmpty() && victim != me)
          task = queue[victim]->popTop();

        if (task != nullptr)
          printf("work stole %d --> %d\n", victim, me);
      }
    } else
      return;
    std::this_thread::yield();
  }
}

} // namespace wsq

#endif
