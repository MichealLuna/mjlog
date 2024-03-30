#include "LogFile.h"
#include "LoggerUtil.h"
#include "Logger.h"
#include <cassert>

using namespace ZMJ::Log;

LogFile::LogFile(std::string basename_, off64_t rollSize_, bool threadSafe_, int flushInterval_, int checkEveryN_) : 
    basename(basename_),                                                                                                           
    rollSize(rollSize_),
    flushInterval(flushInterval_),
    checkEveryN(checkEveryN_),
    theSameFilenameCount(0),
    mtx(threadSafe_ ? std::make_unique<std::mutex>() : nullptr)
{
    rollFile();
}

int LogFile::append(const char *line, int len)
{
    int ret = 0;
    if (mtx)
    {
        std::unique_lock<std::mutex> lock(*mtx);
        ret = appendUnlocked(line, len);
    }
    else
    {
        ret = appendUnlocked(line, len);
    }
    return ret;
}

void LogFile::flush()
{
    if (mtx)
    {
        std::unique_lock<std::mutex> lock(*mtx);
        file->flush();
    }
    else
    {
        file->flush();
    }
}

/*
* 这个函数有个bug：就是根据time函数获取的时间只精确到秒
*                对于计算机来说，1秒完全可以写入大于给定的rollSize大小的日志。
*                对于实际环境，当然1秒就写入几十M的日志情况比较少，但是这也是
*                可能的。它也不算太大bug，也就导致日志文件大于给定的rollSize。
*/
void LogFile::rollFile(const time_t *cacheNow)
{
    time_t now;
    if (cacheNow != nullptr)
    {
        now = *cacheNow;
    }
    else
    {
        now = time(nullptr);
    }

    auto filename = Util::getLogFilename(basename, now);
    auto start = now / RollPerSeconds * RollPerSeconds; /*表示从1900年至今天的 00 : 00 的秒数。*/ 
    trace("now %ld, last roll: %ld",now,lastRoll);

    /*如果时间相同，说明1秒内写入了很多日志，需要分文件。但如果加这个代码，会多产生大概六分之一的开销。*/
    if(now == lastRoll)
    {
        char tmp[64] = {0};
        snprintf(tmp,sizeof(tmp),".%d",++theSameFilenameCount);
        filename += tmp;
    }

    trace("new log filename %s",filename.c_str());

    if (now >= lastRoll)
    {
        lastRoll = now;
        lastFlush = now;
        lastPeriod = start;
        file = std::make_unique<FileAppender>(filename);
    }
}

int LogFile::appendUnlocked(const char *line, int len)
{
    /*
     *   第一优先级是日志文件大小，如果到达限制大小，马上进行日志滚动。
     *   第二优先级是行数，当至少有checkEveryN行写入日志时，才会发生下面事件：
     *       1、每flushInterval秒会进行刷盘；（如果没有checkEveryN行，那么每3秒就不会刷盘）
     *       2、如果当前正好是新一天，那么就更换新日志文件，(同理，没有checkEveryN行日志不会更换日志）。
     */

    int ret = file->append(line, len);
    if (file->writtenBytes() >= rollSize)
    { // 这是如果当前文件写入大小超过rollSize，保存并另外新建一个文件。
        trace("最多写入%dbytes,当前已写入%ldbytes.",rollSize,file->writtenBytes());
        rollFile();
        file->resetWrittenBytes();
    }
    else
    {
        ++count;
        if (count >= checkEveryN)
        {
            count = 0; //归零
            trace("LogFile chech every %d lines and current written %ld bytes.",checkEveryN,file->writtenBytes());
            time_t now = ::time(nullptr);
            time_t curPeriod = now / RollPerSeconds * RollPerSeconds;
            if (curPeriod != lastPeriod)
            { // 这是不同一天,并且cout 大于 checkEveryN
                rollFile(&now);
            }
            else if (now - lastFlush > flushInterval)
            { // 这是间隔flushInterval刷盘一次。
                lastFlush = now;
                file->flush();
            }
        }
    }
    return ret;
}
