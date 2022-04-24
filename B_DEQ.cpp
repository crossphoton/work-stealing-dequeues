#ifndef _PCP_B_DEQUEUE
#define _PCP_B_DEQUEUE

#include "WSQ.h"

#ifdef __DEBUG_MODE
#include <iostream>
#include <stdio.h>
#endif
namespace wsq {
class BDEQueue : public DEQueue {

public:
  BDEQueue(int capacity) {
    this->tasks = new Runnable[capacity];
    this->top = new AtomicStampedReference<int>(0, 0);
    this->bottom = 0;
  }

  ~BDEQueue() {}

  void pushBottom(Runnable r) {
    this->tasks[bottom] = r;
    bottom++;
  }

  bool isEmpty() { return !(this->top->getReference() < this->bottom); }

  Runnable *popTop() {
    int *stamp = new int;
    int oldTop = top->get(stamp), newTop = oldTop + 1;
    int oldStamp = stamp[0], newStamp = oldStamp + 1;
    if (bottom <= oldTop)
      return nullptr;
    Runnable *r = &tasks[oldTop];
    if (top->compareAndSet(oldTop, newTop, oldStamp, newStamp))
      return r;
    return nullptr;
  }

  Runnable *popBottom() {
    if (bottom == 0)
      return nullptr;

    bottom--;
    Runnable *r = &tasks[bottom];
    int *stamp = new int[1];
    int oldTop = top->get(stamp), newTop = 0;
    int oldStamp = stamp[0], newStamp = oldStamp + 1;
    if (bottom > oldTop)
      return r;
    if (bottom == oldTop) {
      bottom = 0;
      if (top->compareAndSet(oldTop, newTop, oldStamp, newStamp))
        return r;
    }
    top->set(newTop, newStamp);
    return nullptr;
  }

#ifdef __DEBUG_MODE
  void printQ() {
    printf("..................\n");
    for (int i = this->top->getReference(); i < this->bottom; i++) {
      printf("%ld\t", this->tasks[i].id);
    }
    printf("\n..................\n");
  }
#endif
};
} // namespace wsq

#endif
