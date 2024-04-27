/* Hacky attempt at a ring buffer implementation in C */

struct ringbuffer // MY PRECIOUSSSS
{
        void ** storage = nullptr;
        int32_t capacity = 0;
        int32_t start = 0;
        int32_t end = 1; // NOTE: exclusive
};

// TODO: return insert status
void rb_push_elem(ringbuffer * r, void * new_elem)
{
        if (r->start == r->end)
                return;

        r->storage[r->end++] = new_elem;
        r->end %= r->capacity;
}

// Returns popped element
// TODO: error handling
void * rb_pop_elem(ringbuffer * r)
{
        void * tmp = r->storage[r->start++];
        r->start %= r->capacity;
        return tmp;
}

