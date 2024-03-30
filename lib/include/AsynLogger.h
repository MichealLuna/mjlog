#ifndef ASYNLOGGER_H
#define ASYNLOGGER_H

#include "Noncopyable.h"
#include "FixedBuffer.h"
#include "CountDownLatch.h"
#include "LogFile.h"
#include "LoggerUtil.h"

#include <iostream>
#include <memory>
#include <vector>

#include <thread>
#include <atomic>
#include <cassert>

namespace ZMJ
{
    namespace Log
    {
        class AsynLogger : public Noncopyable
        {
        public:
            using Buffer = FixedBuffer<LargeBuffer>;
            using BufferPtr = std::unique_ptr<Buffer>;
            using BufferPtrVector = std::vector<BufferPtr>;

            explicit AsynLogger(const std::string basename_, off64_t rollsize_, int flushInterval_ = 3);

            ~AsynLogger();

            int write(const char *line, size_t len);

            void thread_work();

            // 线程终止回收
            void done();

        private:
            BufferPtr curBuffer;
            BufferPtr nextBuffer;
            BufferPtrVector buffers;
            std::mutex mtx;
            std::condition_variable cond; // for BufferPtrVector
            std::unique_ptr<std::thread> thread;
            std::atomic<bool> isDone;
            CountDownLatch latch;
            const std::string basename;
            const int flushInterval;
            const off64_t rollSize;
        };
    } // namespace Log
} // namespace ZMJ

#endif // AsynLogger.h