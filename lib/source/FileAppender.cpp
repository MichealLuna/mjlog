
#include <unistd.h>
#include <stdexcept>

#include "FileAppender.h"
#include "LoggerUtil.h"
#include <cassert>

using namespace ZMJ;

void FileAppender::flush()
{
    if (m_file)
    {
        ::fflush(m_file);
    }
}

size_t FileAppender::write(const char *line, int len)
{
    size_t sz = 0;
    if (m_file)
    {
        // 这个是
        sz = fwrite_unlocked(line, 1, len, m_file);
    }
    return sz;
}

void FileAppender::init(std::string const &filename)
{
    std::string filedir;
    size_t pos;
    if ((pos = filename.rfind('/')) == std::string::npos)
    {
        throw std::runtime_error("invalid filepath");
    }

    // 之所以+1，是因为substr是左闭右开，也就是不包括pos+1这个位置字符。
    filedir = filename.substr(0, pos + 1);
    if (access(filedir.c_str(), F_OK) == -1)
    {
        throw std::runtime_error("filedir doesn't exist!");
    }

    m_file = fopen(filename.c_str(), "ae");
    if (m_file == nullptr)
    {
        int err = ferror(m_file);
        auto *errorInfo = Util::getErrorInfo(err);
        fprintf(stderr, "FileAppender error in open file:%s erron:%s", filename.c_str(), errorInfo);
        return;
    }
}

FileAppender::FileAppender(const std::string &basename_)
{
    init(basename_);
}

FileAppender::~FileAppender()
{
    if (m_file != nullptr)
    {
        ::fflush(m_file);
        ::fclose(m_file);
    }
}

int FileAppender::append(const char *line, int len)
{
    //assert还是非常有好处的，我的buffer.size函数写反了，导致size小于0，这里给我拦截下来了。
    assert(len >= 0);
    size_t written = 0;

    while (written != len)
    {
        size_t remaining = len - written;
        size_t n = write(line + written, remaining);
        if (n != remaining)
        {
            int err = ferror(m_file);
            if (err)
            {
                fprintf(stderr, "FileAppender::append(const char*,int len) failed %s", Util::getErrorInfo(err));
                break;
            }

            if (n == 0)
            {
                throw std::runtime_error("write error,FILE* is nullptr.");
            }
        }

        written += n;
    }
    m_writtenBytes += written;
    return written;
}

off64_t FileAppender::writtenBytes()
{
    return m_writtenBytes;
}

void FileAppender::resetWrittenBytes()
{
    m_writtenBytes = 0;
}
