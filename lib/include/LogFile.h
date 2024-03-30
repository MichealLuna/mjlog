#ifndef __LOGFILE_H__
#define __LOGFILE_H__

#include "Noncopyable.h"
#include "FileAppender.h"

#include <sys/types.h>
#include <string>
#include <memory>
#include <ctime>
#include <mutex>

namespace ZMJ
{
    namespace Log
    {
        class LogFile : public Noncopyable
        {
        public:
            enum
            {
                RollPerSeconds = 60 * 60 * 24
            };

            LogFile(std::string basename_, off64_t rollSize_, bool threadSafe_ = true, int flushInterval_ = 3, int checkEveryN_ = 1024);
            ~LogFile() = default;

            int append(const char *line, int len);

            void flush();

            void rollFile(const time_t *cacheNow = nullptr);
            
        private:
            int appendUnlocked(const char *line, int len);

        private:
            std::string basename;
            off64_t rollSize;
            const int flushInterval;
            const int checkEveryN;
            int count;
            int theSameFilenameCount; //以秒为单位时相同秒得相同文件，用于区分。

            time_t lastPeriod{};
            time_t lastRoll{};
            time_t lastFlush{};
            std::unique_ptr<std::mutex> mtx;//protect for file
            std::unique_ptr<FileAppender> file;
        };
    } // namespace Log

} // namespace ZMJ

#endif // Logfile.h