#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <sys/types.h>
#include <syscall.h>
#include <unistd.h>
#include <string>

namespace ZMJ
{
    namespace ProcessInfo
    {
        pid_t GetTid();

        pid_t GetPid();

        uid_t GetUid();

        std::string GetHostname();
    } // namespace ProcessInfo

} // namespace ZMJ

#endif // ProcessInfo.h