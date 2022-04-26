#ifndef _PCP_B_DEQUEUE
#define _PCP_B_DEQUEUE

#include "WSQ.cpp"

#ifdef __DEBUG_MODE
#include <iostream>
#include <stdio.h>
#endif
namespace wsq
{
  class BDEQueue : public DEQueue
  {

  public:
    /**
     * It creates a new queue with a given capacity.
     *
     * @param capacity the maximum number of tasks that can be stored in the queue
     */
    BDEQueue(int capacity)
    {
      this->tasks = new Runnable[capacity];
      this->top = new AtomicStampedReference<int>(0, 0);
      this->bottom = 0;
    }

    ~BDEQueue() {}

    /**
     * PushBottom() adds a new task to the bottom of the queue.
     *
     * @param r The runnable object that is to be pushed onto the queue.
     */
    void pushBottom(Runnable r)
    {
      this->tasks[bottom] = r;
      bottom++;
    }

    /**
     * If the top of the queue is not less than the bottom of the queue, then the queue is empty.
     *
     * @return A boolean value.
     */
    bool isEmpty() { return !(this->top->getReference() < this->bottom); }

    Runnable *popTop()
    {
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

    /**
     * If the bottom of the stack is not empty, then pop the bottom element and return it
     *
     * @return A Runnable object.
     */
    Runnable *popBottom()
    {
      if (bottom == 0)
        return nullptr;

      bottom--;
      Runnable *r = &tasks[bottom];
      int *stamp = new int[1];
      int oldTop = top->get(stamp), newTop = 0;
      int oldStamp = stamp[0], newStamp = oldStamp + 1;
      if (bottom > oldTop)
        return r;
      if (bottom == oldTop)
      {
        bottom = 0;
        if (top->compareAndSet(oldTop, newTop, oldStamp, newStamp))
          return r;
      }
      top->set(newTop, newStamp);
      return nullptr;
    }

    void printQ()
    {
      printf("..................\n");
      for (int i = this->top->getReference(); i < this->bottom; i++)
      {
        printf("%ld\t", this->tasks[i].id);
      }
      printf("\n..................\n");
    }
  };
} // namespace wsq

#endif
