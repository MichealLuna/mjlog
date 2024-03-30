#include "LogToConsole.h"
#include <stdio.h>

using namespace ZMJ::Log;

int LogToConsole::write(const char* log,int len){
    flockfile(stdout);
    fprintf(stdout,"%s",log);
    funlockfile(stdout);
    return len;
}