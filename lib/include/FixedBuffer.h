#ifndef FIXEDBUFFER_H
#define FIXEDBUFFER_H

#include <cstring>

#include "Noncopyable.h"

namespace ZMJ
{
    enum
    {
        SmallBuffer = 4096,
        LargeBuffer = 4096 * 1024 // 这是4M
    };

    template <int SIZE>
    class FixedBuffer : public Noncopyable
    {
    public:
        FixedBuffer() : curWriteIndex(buffer) {}

        char *data()
        {
            return buffer;
        }

        int append(const char *line, size_t len)
        {
            if (remainingBuffer() > len)
            {
                memcpy(curWriteIndex, line, len);
                curWriteIndex += len;
                return len;
            }
            return 0;
        }

        void bzero()
        {
            memset(buffer, 0, sizeof(buffer));
        }

        void reset()
        {
            curWriteIndex = buffer;
        }

        int size()
        {
            return static_cast<int>(curWriteIndex - buffer);
        }

        int remainingBuffer()
        {
            return static_cast<int>(end() - curWriteIndex);
        }

    private:
        char *end() { return buffer + sizeof(buffer); }

    private:
        char buffer[SIZE]{};
        char *curWriteIndex{};
    };

} // namespace ZMJ

#endif // FixedBuffer.h