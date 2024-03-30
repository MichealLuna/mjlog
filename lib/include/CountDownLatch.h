#ifndef __COUNT_DOWN_LATCH_H
#define __COUNT_DOWN_LATCH_H

#include <mutex>
#include <condition_variable>

namespace ZMJ
{

class CountDownLatch{
public:
    explicit CountDownLatch(size_t cnt = 1);

    void wait();

    void countDown();

    size_t getCount();
private:
    size_t count;
    std::mutex mtx;
    std::condition_variable cond;    
};

} // namespace ZMJ



#endif //CountDownLatch.h