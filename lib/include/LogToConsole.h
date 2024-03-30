#ifndef __LOG_CONSOLE__
#define __LOG_CONSOLE__

#include "LogTo.h"

namespace ZMJ
{
    namespace Log
    {
        class LogToConsole : public LogTo{
        public:
            LogToConsole() = default;
            ~LogToConsole() = default;
            int write(const char* log,int len) override;
        };
    } // namespace Log
    
} // namespace ZMJ


#endif//LogToConsole.h