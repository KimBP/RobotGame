/*
 * safequeue.cpp
 *
 *  Created on: Oct 25, 2015
 *      Author: kbp
 */



/*
 * Found at
 *   http://stackoverflow.com/questions/15278343/c11-thread-safe-queue
 */

#ifndef SAFE_QUEUE
#define SAFE_QUEUE

#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class SafeQueue
{
public:
  SafeQueue(void)
    : q()
    , m()
    , c()
  {}

  SafeQueue(const SafeQueue& other)
    {
      q = other.q;
    }
  ~SafeQueue(void)
    {}

  void enqueue(T t)
  {
    std::lock_guard<std::mutex> lock(m);
    q.push(t);
    c.notify_one();
  }

  T dequeue(void)
  {
    std::unique_lock<std::mutex> lock(m);
    while(q.empty())
    {
      c.wait(lock);
    }
    T val = q.front();
    q.pop();
    return val;
  }

  unsigned int size() {
	  return q.size();
  }

  bool empty(void)
  {
	 return q.empty();
  }

private:
  std::queue<T> q;
  mutable std::mutex m;
  std::condition_variable c;
};
#endif

