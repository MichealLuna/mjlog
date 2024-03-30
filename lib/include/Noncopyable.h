#ifndef NON_COPY_ABLE
#define NON_COPY_ABLE

namespace ZMJ
{
    class Noncopyable
    {
    public:
        Noncopyable(const Noncopyable &) = delete;
        Noncopyable(Noncopyable &&) = delete;
        Noncopyable &operator=(const Noncopyable &) = delete;
        Noncopyable &operator=(Noncopyable &&) = delete;

    protected:
        Noncopyable() = default;
        ~Noncopyable() = default;
    };
} // namespace ZMJ

#endif // Noncopyable.h