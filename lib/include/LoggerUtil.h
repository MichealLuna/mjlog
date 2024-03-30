#ifndef __LOGGER_UTIL_H__
#define __LOGGER_UTIL_H__

#include <ctime>
#include <string>

namespace ZMJ
{

    class Util
    {
    public:
        static const char *getCurDateTime(bool isTime, time_t *now = nullptr);
        static const char *getErrorInfo(int errorCode);
        static std::string getLogFilename(const std::string &basename, time_t &now);
    };

} // namespace ZMJ

#endif // LoggerUtil.h