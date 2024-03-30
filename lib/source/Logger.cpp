#include <cstdarg>

#include "Logger.h"
#include "ProcessInfo.h"
#include "LogToConsole.h"
#include "AsynLogToFile.h"
#include "SyncLogToFile.h"

using namespace ZMJ::Log;

#define DEFAULT_ROLL_SIZE 20 * 1024 * 1024
#define BUFFER_SIZE 1024 * 1024

static const char *LogLevel[] = {
    "[Debug]:",
    "[Warn]:",
    "[Info]:",
    "[Error]:",
    "[Fatal]:"};

Logger::Logger()
{
    try
    {
        buffer = std::make_unique<char[]>(BUFFER_SIZE); // 指向数组的智能指针
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        throw std::runtime_error("Alloc log buffer Failed.");
    }
}

void Logger::init(const char *file, Level l, size_t rollSize, int conf)
{
    baseFilename = file;
    level = l;
    rollSize_ = rollSize;
    config = conf;

    if (config & ASYN)
    {
        try
        {
            trace("第一次创建AsynLogger,此时不应该有线程正在创建。");
            customLogList.push_back(std::make_shared<AsynLogToFile>(file, rollSize));
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            throw std::runtime_error("AsynLogging Failed.");
        }
    }

    if (config & CONSOLE)
    {
        try
        {
            trace("第一次创建AsynLogger,此时不应该有线程正在创建。");
            customLogList.push_back(std::make_shared<LogToConsole>());
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            throw std::runtime_error("AsynLogging Failed.");
        }
    }
}

void Logger::writeLog(LogContext &ctx, const char *format, ...)
{
    // 如果日志级别低，那么不输出。
    if (level > ctx.level)
        return;
    assert(ctx.level < LEVEL_COUNT);
    va_list vlist;
    va_start(vlist, format);

    int len = 0;
    // time
    len += snprintf(buffer.get(), BUFFER_SIZE, "[Time]:%s ", Util::getCurDateTime(false));
    assert(len < BUFFER_SIZE);

    // filename + line
    if (config & FNAME_LINE)
    {
        len += snprintf(buffer.get() + len, BUFFER_SIZE - len, "at %s", ctx.filename);
        len += snprintf(buffer.get() + len, BUFFER_SIZE - len, ":%d ", ctx.line);
        assert(len < BUFFER_SIZE);
    }

    // UID
    if (config & UID)
    {
        len += snprintf(buffer.get() + len, BUFFER_SIZE - len, "[UID]:%d ", ProcessInfo::GetUid());
        assert(len < BUFFER_SIZE);
    }

    // PID
    if (config & PID)
    {
        len += snprintf(buffer.get() + len, BUFFER_SIZE - len, "[PID]:%d ", ProcessInfo::GetPid());
        assert(len < BUFFER_SIZE);
    }

    // TID
    if (config & TID)
    {
        len += snprintf(buffer.get() + len, BUFFER_SIZE - len, "[TID]:%d ", ProcessInfo::GetTid());
        assert(len < BUFFER_SIZE);
    }

    // level
    len += snprintf(buffer.get() + len, BUFFER_SIZE - len, "%s", LogLevel[ctx.level]);
    assert(len < BUFFER_SIZE);

    // log
    // vsnprintf会留出一个空间放入'\0'，所以这里再预留一个空间用来放换行符
    len += vsnprintf(buffer.get() + len, BUFFER_SIZE - len - 1, format, vlist);
    buffer.get()[len++] = '\n';

    /*到目前为止，buffer里面是以换行符结尾的，对于写入的文件是如此，但对于写入控制台的要义空字符结尾*/
    {
        std::unique_lock<std::mutex> lock(mtx);
        for (auto it = customLogList.begin(); it != customLogList.end(); ++it)
        {
            if (dynamic_cast<LogToConsole *>(it->get()))
            {
                buffer.get()[len++] = '\0';
                (*it)->write(buffer.get(), len);
                continue;
            }
            it->get()->write(buffer.get(), len);
        }
    }

    va_end(vlist);
}

void ZMJ::Log::internalLog(const char *format, ...)
{
    va_list vlist;
    va_start(vlist, format);

    char buf[512] = {};

    int len = 0;
    len += vsnprintf(buf, sizeof(buf) - 1, format, vlist);
    buf[len++] = '\n';
    buf[len] = '\0';
    flockfile(stdout);
    fprintf(stdout, "%s", buf);
    funlockfile(stdout);
    va_end(vlist);
}

void Logger::addCustomLog(LogPtr logTo)
{
    std::unique_lock<std::mutex> lock(mtx);
    customLogList.push_back(logTo);
}

void Logger::delCustomLog(LogPtr logTo)
{
    std::unique_lock<std::mutex> lock(mtx);
    for (auto it = customLogList.begin(); it != customLogList.end(); ++it)
    {
        if (it->get() == logTo.get())
        {
            it = customLogList.erase(it);
            break;
        }
    }
}

LogPtr Logger::registerConsolLog()
{
    std::shared_ptr<LogToConsole> tmp = std::make_shared<LogToConsole>();
    std::shared_ptr<Log> base = std::dynamic_pointer_cast<Log>(tmp);
    {
        std::unique_lock<std::mutex> lock(mtx);
        customLogList.push_back(base);
    }
    return base;
}

LogPtr Logger::registerAsynLog(const char *file, size_t rollSize)
{
    std::shared_ptr<AsynLogToFile> tmp = std::make_shared<AsynLogToFile>(file, rollSize);
    std::shared_ptr<Log> base = std::dynamic_pointer_cast<Log>(tmp);
    {
        std::unique_lock<std::mutex> lock(mtx);
        customLogList.push_back(base);
    }
    return base;
}

LogPtr Logger::registerSyncLog(const char *file, size_t rollSize)
{
    std::shared_ptr<SyncLogToFile> tmp = std::make_shared<SyncLogToFile>(file, rollSize);
    std::shared_ptr<Log> base = std::dynamic_pointer_cast<Log>(tmp);
    {
        std::unique_lock<std::mutex> lock(mtx);
        customLogList.push_back(base);
    }
    return base;
}