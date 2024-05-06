#include <string.h>
/* Hacky attempt at a ring buffer implementation in C */

// TODO: init function
typedef struct // MY PRECIOUSSSS
{
        void ** storage;
        int32_t capacity;
        int32_t start;
        int32_t end; // NOTE: exclusive
} ringbuffer;

// TODO: return insert status
void rb_push_elem(ringbuffer * r, void * new_elem)
{
        r->storage[r->end++] = new_elem;
        r->end %= r->capacity;
}

void rb_push_bulk(ringbuffer * r, void ** new_elems, int32_t count)
{
        if (r->start == r->end)
                return;

        // One-part copy
        if (r->start > r->end)
        {
                // No max function in C lmao
                const int32_t effective_count = count > r->start - r->end ? r->start - r->end : count;
                memcpy(r->storage + r->end, new_elems, effective_count * sizeof(void *));
                // TODO: break into a function
                r->end = (r->end + effective_count) % r->capacity;
                return;
        }

        // Two-part copy
        const int32_t first_copy_count = r->capacity - r->end > count ? count : r->capacity - r->end;
        memcpy(r->storage + r->end, new_elems, first_copy_count * sizeof(void *));
        r->end = (r->end + first_copy_count) % r->capacity;
        if (first_copy_count >= count)
                return;

        const int32_t second_copy_count = r->start < count - first_copy_count ? r->start : count - first_copy_count;
        memcpy(r->storage, new_elems + first_copy_count, second_copy_count * sizeof(void *));
        r->end = (r->end + second_copy_count) % r->capacity;
}

// Returns popped element
// TODO: error handling
void * rb_pop_elem(ringbuffer * r)
{
        void * tmp = r->storage[r->start++];
        r->start %= r->capacity;
        return tmp;
}

