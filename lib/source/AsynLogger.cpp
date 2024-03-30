#include "AsynLogger.h"
#include "Logger.h"

using namespace ZMJ::Log;

AsynLogger::AsynLogger(const std::string basename_, off64_t rollsize_, int flushInterval_) : 
    mtx(),
    cond(),
    isDone(false),
    latch(1),
    basename(std::move(basename_)),
    flushInterval(flushInterval_),
    rollSize(rollsize_)
{
    try
    {
        curBuffer = std::make_unique<Buffer>();
        nextBuffer = std::make_unique<Buffer>();
        trace("主线程正在创建后台线程...");
        thread = std::make_unique<std::thread>([this]()
                                               { thread_work(); });
        trace("主线程正在等待后台线程初始化完毕...");
        latch.wait();
        trace("主线程正在后台线程初始化完毕...");
    }
    catch (const std::exception &e)
    {
        trace("主线程正在buffer创建异常...");
        done();
        throw std::runtime_error("AsyncLogging create thread or buffer alloc error.");
    }
}

AsynLogger::~AsynLogger()
{
    trace("主线程AsynLogger正在析构...");
    done();
}

int AsynLogger::write(const char *line, size_t len)
{
    std::unique_lock<std::mutex> lock(mtx);
    // 陷阱：如果remainBuffer()返回负值，可能导致条件成立！想了想还是加了一个assert，如果调试会知道什么问题，但这种一般是随机bug，assert用还是不大。
    int remain = curBuffer->remainingBuffer();
    assert(remain >= 0);

    if (static_cast<size_t>(remain) > len)
    {
        return curBuffer->append(line, len);
    }

    // 这里并不用担心curBuffer和nextBuffer，因为后台线程会进行更新。
    buffers.push_back(std::move(curBuffer));
    if (!nextBuffer)
    {
        curBuffer = std::move(nextBuffer);
    }
    else
    {
        try
        {
            curBuffer = std::make_unique<Buffer>();
        }
        catch (const std::exception &e)
        {
            done();
            throw std::runtime_error(e.what());
        }
    }
    int ret = curBuffer->append(line, len);
    cond.notify_one();
    return ret;
}

void AsynLogger::thread_work()
{
    try
    {
        LogFile file(basename, rollSize, false);
        BufferPtr buffer1 = std::make_unique<Buffer>();
        BufferPtr buffer2 = std::make_unique<Buffer>();
        buffer1->bzero();
        buffer2->bzero();

        BufferPtrVector buffersForWrite;
        buffersForWrite.reserve(16);

        bool once = false;

        while (!isDone)
        {
            {
                std::unique_lock<std::mutex> lock(mtx);
                trace("后台线程获取锁成功...");
                if (buffers.empty())
                {
                    if (!once)
                    {
                        trace("后台线程初始化完毕...");
                        once = true;
                        latch.countDown();
                    }
                    cond.wait_for(lock, std::chrono::seconds(flushInterval));
                    trace("后台线程被唤醒或超时...");
                }
                buffers.push_back(std::move(curBuffer));
                curBuffer = std::move(buffer1);
                buffers.swap(buffersForWrite);
                if (!nextBuffer)
                {   
                    trace("后台线程更新nextBuffer...");
                    nextBuffer = std::move(buffer2);
                }
            }

            // 貌似不可能出现这种情况，如果buffers之前已经push_back了，只不过curBuffer可能是空的。
            if (buffersForWrite.empty())
                continue;

            if (buffersForWrite.size() > 25)
            {
                char buf[256];
                snprintf(buf, sizeof(buf), "Dropped log messages at %s,%zd larger buffers\n",
                         Util::getCurDateTime(true), buffersForWrite.size() - 2);
                buffersForWrite.erase(buffersForWrite.begin() + 2, buffersForWrite.end());
            }

            // 这里打包好的日志，交给下层去写。
            for (const auto &buffer : buffersForWrite)
            {
                file.append(buffer->data(), buffer->size());
            }

            if (buffersForWrite.size() > 2)
            {
                buffersForWrite.resize(2);
            }

            if (!buffer1)
            {
                assert(!buffersForWrite.empty());
                buffer1 = std::move(buffersForWrite.back());
                buffersForWrite.pop_back();
                buffer1->reset();
            }

            if (!buffer2)
            {
                assert(!buffersForWrite.empty());
                buffer2 = std::move(buffersForWrite.back());
                buffersForWrite.pop_back();
                buffer2->reset();
            }

            buffersForWrite.clear();
            file.flush();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "thread abnormal exit: " << e.what() << '\n';
        thread.reset();
    }
}

// 线程终止回收
void AsynLogger::done()
{
    isDone = true;
    trace("唤醒阻塞线程...");
    cond.notify_one();
    if (thread && thread->joinable())
    {
        trace("等待后台线程清理资源...");
        thread->join();
    }
}