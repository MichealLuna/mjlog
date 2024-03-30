#include "AsynLogToFile.h"

using namespace ZMJ::Log;

AsynLogToFile::AsynLogToFile(const std::string basename_, off64_t rollsize_, int flushInterval_):
    asynLog(std::make_unique<AsynLogger>(basename_,rollsize_,flushInterval_))
{
}

int AsynLogToFile::write(const char* log,int len){
    return asynLog->write(log,len);
}