#ifndef _PCP_UNB_DEQUEUE
#define _PCP_UNB_DEQUEUE

#include "B_DEQ.cpp"

#ifdef __DEBUG_MODE
#include <stdio.h>
#endif

namespace wsq
{

  class CircularArray
  {
    int logCapacity;
    Runnable *currentTasks;

  public:
    CircularArray(int myLogCapacity)
    {
      this->logCapacity = myLogCapacity;
      this->currentTasks = new Runnable[1 << logCapacity];
    }

    int capacity() { return 1 << logCapacity; }

    Runnable *get(int i) { return &currentTasks[i % capacity()]; }

    void put(int i, Runnable task) { currentTasks[i % capacity()] = task; }

    CircularArray *resize(int bottom, int top)
    {
      CircularArray *newTasks = new CircularArray(logCapacity + 1);
      for (int i = top; i < bottom; i++)
        newTasks->put(i, *get(i));

      return newTasks;
    }
  };

  class UnboundedDEQueue : public DEQueue
  {
  private:
    static int LOG_CAPACITY;
    CircularArray *tasks; // Removed volatile

  public:
    volatile int bottom;
    UnboundedDEQueue(int _LOG_CAPACITY)
    {
      LOG_CAPACITY = _LOG_CAPACITY;
      tasks = new CircularArray(_LOG_CAPACITY);
      top = new AtomicStampedReference<int>(0);
      this->bottom = 0;
    }
    bool isEmpty()
    {
      int localTop = top->getReference();
      int localBottom = this->bottom;
      return (localBottom <= localTop);
    }

    /**
     * "If the queue is full, resize it, then add the new task to the bottom of the queue."
     *
     * The first thing we do is get the current top and bottom of the queue. We then get the current array
     * of tasks. We then check to see if the queue is full. If it is, we resize the queue. We then add the
     * new task to the bottom of the queue
     *
     * @param r The runnable to push
     */
    void pushBottom(Runnable r)
    {
      int oldBottom = this->bottom;
      int oldTop = top->getReference();
      CircularArray *currentTasks = tasks;
      int size = oldBottom - oldTop;
      if (size >= currentTasks->capacity() - 1)
      {
        currentTasks = currentTasks->resize(oldBottom, oldTop);
        tasks = currentTasks;
      }
      tasks->put(oldBottom, r);
      bottom = oldBottom + 1;
    }

    /**
     * If the top of the queue is less than the bottom of the queue, then return the top of the queue
     *
     * @return A Runnable object.
     */
    Runnable *popTop()
    {
      int oldTop = top->getReference();
      int newTop = oldTop + 1;
      int oldBottom = bottom;

      CircularArray *currentTasks = tasks;
      int size = oldBottom - oldTop;
      if (size <= 0)
        return nullptr;
      Runnable *r = tasks->get(oldTop);
      if (top->compareAndSet(oldTop, newTop, 0, 0))
        return r;
      return nullptr;
    }

    /**
     * If the queue is empty, return nullptr. Otherwise, return the bottom element of the queue
     *
     * @return A Runnable object.
     */
    Runnable *popBottom()
    {
      CircularArray *currentTasks = tasks;
      bottom--;
      int oldTop = top->getReference();
      int newTop = oldTop + 1;
      int size = bottom - oldTop;
      if (size < 0)
      {
        bottom = oldTop;
        return nullptr;
      }
      Runnable *r = tasks->get(bottom);
      if (size > 0)
        return r;
      if (!top->compareAndSet(oldTop, newTop, 0, 0))
        r = nullptr;
      bottom = oldTop + 1;
      return r;
    }
    void printQ()
    {
      printf("..................\n");
      for (int i = top->getReference(); i < bottom; i++)
      {
        printf("%ld\t", this->tasks->get(i)->id);
      }
      printf("\n..................\n");
    }
  };

  int UnboundedDEQueue::LOG_CAPACITY = 4;
} // namespace wsq

#endif
