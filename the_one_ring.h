#include <stdbool.h>
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
        *r = (ringbuffer){
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

bool rb_full(const ringbuffer * r)
{
        return r->count == r->capacity;
}

bool rb_empty(const ringbuffer * r)
{
        return !r->count;
}

// Returns number of elements inserted
// Will not push if rb full
int32_t rb_push_elem(ringbuffer * r, void * new_elem)
{
        if (rb_full(r))
                return 0;

        r->storage[r->count++] = new_elem;
        return 1;
}

// Returns number of elements inserted
// Will not push past rb fullness
int32_t rb_push_bulk(ringbuffer * r, void ** new_elems, int32_t count)
{
        /*
        if (rb_full(r))
                return 0;

        const int32_t tail_qty = (r->start + r->count) > r->capacity ?
                0 : r->capacity - r->start - r->count;

        const int32_t head_qty = count - tail_qty > r->capacity - r->count ? 
*/
        return 0;
}

// TODO: pop bulk

// Returns popped element
void * rb_pop_elem(ringbuffer * r)
{
        if (rb_empty(r))
                return NULL;

        void * tmp = r->storage[r->start++];
        r->start %= r->capacity;
        r->count--;
        return tmp;
}
