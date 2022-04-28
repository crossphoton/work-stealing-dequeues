#ifndef _PCP_WS_QUEUE
#define _PCP_WS_QUEUE

#include <atomic>
#include <functional>
#include <stdio.h>
#include <thread>
#include <unistd.h>

namespace wsq
{

  template <typename T>
  struct AtomicStamp
  {
    T value;
    int timestamp;
    AtomicStamp(T data) noexcept : value(data), timestamp(0) {}
    AtomicStamp() noexcept : timestamp(0) {}
    AtomicStamp(T data, int stamp) noexcept : value(data), timestamp(stamp) {}
    bool operator==(const AtomicStamp<T> &other) const
    {
      return value == other.value && timestamp == other.timestamp;
    }
  };

  template <typename T>
  class AtomicStampedReference
  {

  public:
    std::atomic<AtomicStamp<T>> snap;
    AtomicStampedReference() {}
    AtomicStampedReference(T data) { snap.store(AtomicStamp<T>(data)); }
    AtomicStampedReference(T data, int stamp)
    {
      snap.store(AtomicStamp<T>(data, stamp));
    }

    bool compareAndSet(T oldRef, T newRef, int oldStamp, int newStamp)
    {
      AtomicStamp<T> old = this->snap.load();
      AtomicStamp<T> newVal = AtomicStamp<T>(newRef, newStamp);
      return snap.compare_exchange_strong(old, newVal, std::memory_order_release,
                                          std::memory_order_relaxed);
    }

    T get(int *ptr)
    {
      AtomicStamp<T> temp = snap.load();

      ptr[0] = temp.timestamp;
      return temp.value;
    }
    T getReference() { return snap.load().value; }
    int getStamp() { return snap.load().timestamp; }
    void set(T data)
    {
      AtomicStamp<T> temp = AtomicStamp<T>(data);
      snap.store(temp);
    }
    void set(T data, int stamp)
    {
      AtomicStamp<T> temp = AtomicStamp<T>(data, stamp);
      snap.store(temp);
    }
  };

  long __runnable_id = 0;
  class Runnable
  {
    long _gen_id() { return ++__runnable_id; }

  public:
    long id;
    std::function<void()> task = NULL;
    template <typename func, typename... Args>
    Runnable(func f, Args... args)
    {
      id = _gen_id();
      this->task = [=]()
      { return f(args...); };
    }
    void run()
    {
      if (task != NULL)
        task();
    }
    Runnable() {}
  };

  /* A DEQueue is a double-ended queue of Runnables. */
  class DEQueue
  {
  public:
    Runnable *tasks;
    volatile int bottom;
    AtomicStampedReference<int> *top;
    virtual void pushBottom(Runnable r) = 0;
    virtual bool isEmpty() = 0;
    virtual Runnable *popTop() = 0;
    virtual Runnable *popBottom() = 0;
  };

  class WorkStealingThread
  {
    DEQueue **queue;
    int me;
    int totalQueues;
    bool enable;

  public:
    WorkStealingThread() {}
    /**
     * This function is the constructor for the WorkStealingThread class. It initializes the thread's id,
     * the queue array, the number of queues, and the enable_ws flag
     *
     * @param me The thread id of the thread.
     * @param queue The array of queues that the thread will steal from.
     * @param n number of threads
     * @param enable_ws This is a boolean variable that is set to true if you want to enable work stealing.
     */
    WorkStealingThread(int me, DEQueue **queue, int n, bool enable_ws)
    {
      this->me = me;
      this->queue = queue;
      this->totalQueues = n;
      this->enable = enable_ws;
      srand(0);
    }
    int get_thread_id() { return this->me; }
    /**
     * This function is the runner function. It first pops a task from the bottom of its own queue. If the
     * queue is empty, it tries to steal a task from the top from other queues.
     *
     * @return A pointer to a function that takes no arguments and returns void.
     */
    void run()
    {
      // printf("starting work steal %d\n", me);
      Runnable *task = queue[me]->popBottom();
      int count = 0;
      while (true)
      {
        while (task != nullptr)
        {
          task->run();
          task = queue[me]->popBottom();
          count++;
        }
        if (!enable)
          break;
        while (task == nullptr)
        {
          std::this_thread::yield();
          // sleep(0);
          int victim = 0;
          for (; victim < totalQueues; victim++)
          {
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
        sleep(0);
      }
    }
  };
}

#endif
