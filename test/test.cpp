#include <Logger.h>

using namespace ZMJ::Log;

int main(){
    auto asynLog = LOG_INSTANCE.registerAsynLog("./AsynLog");
    auto syncLog = LOG_INSTANCE.registerSyncLog("./SyncLog");
    auto consoleLog = LOG_INSTANCE.registerConsolLog();

    for(int i = 0; i < 100; ++i){
        if(i == 50){
            LOG_INSTANCE.delCustomLog(syncLog);
        }
        if(i == 60){
            LOG_INSTANCE.delCustomLog(consoleLog);
        }
        if(i == 70){
            LOG_INSTANCE.delCustomLog(asynLog);
        }
        DEBUG("line %d This is a debug message.",i);
        INFO("line %d This is a INFO.",i);
        ERROR("line %d This is a ERROR.",i);
        WARN("line %d This is a WARN.",i);
        FATAL("line %d This is a FATAL.",i);
    }
    return 0;
}