#ifndef LOGGER_H
#define LOGGER_H

/*****************************************************************************************/
/*
 * 日志模块：
 *   1、可以开启同步0，或异步1
 *   2、支持日志级别 ： Debug < Warn < Info < Error < Fatal 五个级别。
 *   3、支持：文件、行号、时间。可选：线程ID、进程PID。
 *   4、支持：分文件存储(最大日志大小）、日志滚动(同一天日志至少达到给定行数才进行翻滚)。
 *   5、支持：给定时间间隔刷新日志。
 *   6、格式：【时间】 【文件】 【行号】【用户ID】 【进程ID】 【线程ID】【Level】：
 *   7、可以扩展控制台输出、日志文件输出、网络输出。
 *
 *****************************************************************************************/

#include "Noncopyable.h"
#include "AsynLogger.h"
#include "LogTo.h"

#include <string>
#include <list>

namespace ZMJ
{
    namespace Log
    {
        enum Config
        {
            ASYN = 1,
            CONSOLE = 1 << 1,
            PID = 1 << 2,
            TID = 1 << 3,
            UID = 1 << 4,
            FNAME_LINE = 1 << 5,
        };
        enum Level
        {
            DEBUG = 0,
            WARN,
            INFO,
            ERROR,
            FATAL,
            LEVEL_COUNT
        };
        struct LogContext
        {
            int level;
            int line;
            const char *filename;
        };

        void internalLog(const char *format, ...);

        using Log = LogTo;
        using LogPtr = std::shared_ptr<Log>;

        class Logger : public Noncopyable
        {
        public:
            void init(const char *file = DEFAULT_LOG_NAME, Level l = INFO, size_t rollSize = DefaultRollSize,int conf = ASYN | FNAME_LINE | TID);

            static Logger &getInstance()
            {
                static Logger instance;
                return instance;
            }

            void writeLog(LogContext &ctx, const char *fromat, ...);

            void consoleLog(const char *log, int len);

            void addCustomLog(LogPtr logTo);

            void delCustomLog(LogPtr logTo);

            LogPtr registerConsolLog();

            LogPtr registerAsynLog(const char* file = DEFAULT_LOG_NAME,size_t rollSize = DefaultRollSize);

            LogPtr registerSyncLog(const char* file = DEFAULT_LOG_NAME,size_t rollSize = DefaultRollSize);
        private:
            ~Logger() = default;
            Logger();

        private:
            std::string baseFilename;
            Level level;
            size_t rollSize_;
            int config;

            std::mutex mtx;
            std::list<LogPtr> customLogList;
            std::unique_ptr<char[]> buffer;
        };
    } // namespace Log
} // namespace ZMJ

#define LOG_INSTANCE ZMJ::Log::Logger::getInstance()
#define LOG_INIT(file, level, rollSize, conf) ZMJ::Log::Logger::getInstance().init(file, level, rollSize, conf);

#define DEBUG(format, ...)                                 \
    do                                                     \
    {                                                      \
        ZMJ::Log::LogContext ctx{};                        \
        ctx.level = ZMJ::Log::DEBUG;                       \
        ctx.filename = __FILE__;                           \
        ctx.line = __LINE__;                               \
        LOG_INSTANCE.writeLog(ctx, format, ##__VA_ARGS__); \
    } while (false);

#define WARN(format, ...)                                  \
    do                                                     \
    {                                                      \
        ZMJ::Log::LogContext ctx{};                        \
        ctx.level = ZMJ::Log::WARN;                        \
        ctx.filename = __FILE__;                           \
        ctx.line = __LINE__;                               \
        LOG_INSTANCE.writeLog(ctx, format, ##__VA_ARGS__); \
    } while (false);

#define INFO(format, ...)                                  \
    do                                                     \
    {                                                      \
        ZMJ::Log::LogContext ctx{};                        \
        ctx.level = ZMJ::Log::INFO;                        \
        ctx.filename = __FILE__;                           \
        ctx.line = __LINE__;                               \
        LOG_INSTANCE.writeLog(ctx, format, ##__VA_ARGS__); \
    } while (false);

#define ERROR(format, ...)                                 \
    do                                                     \
    {                                                      \
        ZMJ::Log::LogContext ctx{};                        \
        ctx.level = ZMJ::Log::ERROR;                       \
        ctx.filename = __FILE__;                           \
        ctx.line = __LINE__;                               \
        LOG_INSTANCE.writeLog(ctx, format, ##__VA_ARGS__); \
    } while (false);

#define FATAL(format, ...)                                 \
    do                                                     \
    {                                                      \
        ZMJ::Log::LogContext ctx{};                        \
        ctx.level = ZMJ::Log::FATAL;                       \
        ctx.filename = __FILE__;                           \
        ctx.line = __LINE__;                               \
        LOG_INSTANCE.writeLog(ctx, format, ##__VA_ARGS__); \
    } while (false);

#ifdef TRACE_
  // trace用于跟踪logger日志系统内部情况
#define trace(fmt_, ...)                            \
    do                                              \
    {                                               \
        ZMJ::Log::internalLog(fmt_, ##__VA_ARGS__); \
    } while (false)
#else
#define trace(format, args...)
#endif

#endif // Logge.h