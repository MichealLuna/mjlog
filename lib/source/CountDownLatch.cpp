#include "CountDownLatch.h"

using namespace ZMJ;

CountDownLatch::CountDownLatch(size_t cnt) : count(cnt),
                                             mtx(),
                                             cond()
{
}

void CountDownLatch::wait()
{
    std::unique_lock<std::mutex> lock(mtx);
    while (count > 0)
        cond.wait(lock);
}

void CountDownLatch::countDown()
{
    std::unique_lock<std::mutex> lock(mtx);
    --count;
    if (count == 0)
        cond.notify_all();
}

size_t CountDownLatch::getCount()
{
    std::unique_lock<std::mutex> lock(mtx);
    return count;
}