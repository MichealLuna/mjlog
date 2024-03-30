#include "SyncLogToFile.h"

using namespace ZMJ::Log;

SyncLogToFile::SyncLogToFile(const std::string basename_, off64_t rollsize_,
                            bool threadSafe_,int flushInterval_,int checkEveryN_):
    logFile(std::make_unique<LogFile>(basename_,rollsize_,threadSafe_,flushInterval_,checkEveryN_))
{

}

int SyncLogToFile::write(const char* log,int len) {
    int ret = logFile->append(log,len);
    logFile->flush();
    return ret;
}