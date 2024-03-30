#ifndef __ASYN_LOG_TO_FILE__
#define __ASYN_LOG_TO_FILE__

#include "LogToFile.h"
#include "AsynLogger.h"

namespace ZMJ
{
    namespace Log
    {
        class AsynLogToFile : public LogToFile{
        public:
            explicit AsynLogToFile(const std::string basename_ = DEFAULT_LOG_NAME, off64_t rollsize_ = DefaultRollSize, int flushInterval_ = 3);
            ~AsynLogToFile() = default;
            int write(const char* log,int len) override;
        private:
            std::unique_ptr<AsynLogger> asynLog;
        };
    } // namespace Log{}
        
} // namespace ZMJ


#endif //AsynLogToFile.h