#ifndef __LOG_TO_FILE__
#define __LOG_TO_FILE__

#include "LogTo.h"

namespace ZMJ
{
    namespace Log
    {
        class LogToFile : public LogTo{
        public:
            LogToFile() = default;
            ~LogToFile() = default;
            int write(const char* log,int len) = 0;
        };
    } // namespace Log
    
} // namespace ZMJ


#endif//LogToFile.h