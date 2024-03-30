#ifndef __SYNC_LOG_TO_FILE_H__
#define __SYNC_LOG_TO_FILE_H__

#include "LogToFile.h"
#include "LogFile.h"

namespace ZMJ
{
    namespace Log
    {
        class SyncLogToFile : public LogTo{
        public:
            explicit SyncLogToFile(std::string basename_ = DEFAULT_LOG_NAME, off64_t rollSize_ = DefaultRollSize,
                                     bool threadSafe_ = true, int flushInterval_ = 3, int checkEveryN_ = 1024);
            ~SyncLogToFile() = default;
            int write(const char* log,int len) override;
        private:
            std::unique_ptr<LogFile> logFile;
        };
    } // namespace Log
    
} // namespace ZMJ


#endif//SyncLogToFile.h