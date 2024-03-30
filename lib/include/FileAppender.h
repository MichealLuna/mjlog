#ifndef __FILE_APPENDER_H__
#define __FILE_APPENDER_H__

#include "Noncopyable.h"

#include <string>
#include <cstdio>
#include <sys/types.h>
#include <stdio.h>

namespace ZMJ
{

    class FileAppender : public Noncopyable
    {
    public:
        FileAppender(const std::string &basename_);
        ~FileAppender();

        int append(const char *line, int len);
        void flush();

        off64_t writtenBytes();
        void resetWrittenBytes();

    private:
        size_t write(const char *line, int len);
        void init(std::string const &);

    private:
        FILE *m_file;
        char m_buffer[1024 * 64]; // 为什么这里又搞一个缓冲区，为了一次性写入更多数据，减少系统调用。
        off64_t m_writtenBytes{};
    };

} // namespace ZMJ

#endif // FileAppender.h