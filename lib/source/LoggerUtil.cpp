#include "LoggerUtil.h"
#include "ProcessInfo.h"

using namespace ZMJ;

/*
 * C标准库的时间库
 * time(nullptr)获取当前时间戳。
 * localtime_r(&timer,&tim)；将时间戳转为带年月日小时分钟秒信息填入结构体tim。
 * strftime(timeBuf,sizeof(timeBuf),".%Y%m%d-%H%M%S.",&tim),给定tim时间结构体和格式，填充缓冲。
 */

#include <cstring>
#include <cstdio>
#include <cstring>
#include <cassert>

__thread char errorBuf[512];
__thread char timeBuf[64];
__thread struct tm tim;
__thread time_t lastSecond;

const char *Util::getCurDateTime(bool isTime, time_t *now)
{
    time_t timer = time(nullptr);
    if (now != nullptr)
    {
        *now = timer;
    }

    // 不同时间戳才更新struct tm。
    if (lastSecond != timer)
    {
        lastSecond = timer;
        localtime_r(&timer, &tim);
    }

    int len;

    if (!isTime)
    {
        len = snprintf(timeBuf, sizeof(timeBuf), "%04d-%02d-%02d %02d:%02d:%02d",
                       tim.tm_year + 1900, tim.tm_mon + 1,
                       tim.tm_mday, tim.tm_hour, tim.tm_min, tim.tm_sec);
        assert(len == 19);
    }
    else
    {
        len = snprintf(timeBuf, sizeof(timeBuf), "%04d-%02d-%02d",
                       tim.tm_year + 1900, tim.tm_mon + 1, tim.tm_mday);
        assert(len == 10);
    }
    return timeBuf;
}

const char *Util::getErrorInfo(int errorCode)
{
    return strerror_r(errorCode, errorBuf, sizeof(errorBuf));
}

std::string Util::getLogFilename(const std::string &basename, time_t &now)
{
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;
    char timeBuf[32] = {0};
    struct tm tim;
    localtime_r(&now, &tim);
    // Y表示4位数年份，而y表示2位数年份，m表示1位数月份，d表示1位数月份，H、M、S则表示两位数时分秒，
    strftime(timeBuf, sizeof(timeBuf), ".%Y%m%d-%H%M%S.", &tim);
    filename += timeBuf;
    filename += ProcessInfo::GetHostname();

    char pidBuf[32];
    snprintf(pidBuf, sizeof(pidBuf), ".%d", ProcessInfo::GetPid());
    filename += pidBuf;
    return filename;
}