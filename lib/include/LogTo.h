#ifndef __LOG_TO__
#define __LOG_TO__

#define DEFAULT_LOG_NAME "./MJLOG"

namespace ZMJ
{
    namespace Log
    {
        enum
        {
            DefaultRollSize = 20 * 1024 * 1024
        };
        class LogTo
        {
        public:
            LogTo() = default;
            virtual ~LogTo() = default;
            virtual int write(const char *log, int len) = 0;
        };
    } // namespace Log

} // namespace ZMJ

#endif // LogTo.h