/* Hacky attempt at a ring buffer implementation in C */

struct RingBuffer
{
        void * storage = nullptr;
        int32_t capacity = 0;
        int32_t start = 0;
        int32_t end = 0;
};

