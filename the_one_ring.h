#include <stdlib.h>
#include <string.h>
/* Hacky attempt at a ring buffer implementation in C */

typedef struct // MY PRECIOUSSSS
{
        void ** storage;
        int32_t capacity;
        int32_t start;
        int32_t count;
} ringbuffer;

void rb_create(ringbuffer * r, int32_t capacity_desired)
{
        *r = (ringbuffer) {
                .storage = malloc(capacity_desired * sizeof(void *)),
                .capacity = capacity_desired,
                .start = 0,
                .count = 0,
        };
}

void rb_cleanup(ringbuffer * r)
{
        free(r->storage);
}

// TODO: return insert status
void rb_push_elem(ringbuffer * r, void * new_elem)
{
        // TODO: error handling
        r->storage[r->count++] = new_elem;

        if (r->count > r->capacity)
        {
                r->count = r->capacity;
                r->start++;
        }
}

void rb_push_bulk(ringbuffer * r, void ** new_elems, int32_t count)
{
        // TODO: rewrite
}

// Returns popped element
// TODO: error handling
void * rb_pop_elem(ringbuffer * r)
{
        if (r->count == 0)
                return NULL;

        void * tmp = r->storage[r->start++];
        r->start %= r->capacity;
        r->count--;
        return tmp;
}

