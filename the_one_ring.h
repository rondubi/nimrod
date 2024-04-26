/* Hacky attempt at a ring buffer implementation in C */

struct ringbuffer // MY PRECIOUSSSS
{
        void * storage = nullptr;
        int32_t capacity = 0;
        int32_t start = 0;
        int32_t end = 0;
};

// NOTE: these two methods return the number of elements inserted
// TODO: error handling
int32_t insert_bulk(ringbuffer * r, void * new_elems, int32_t count)
{
        if (r.end == r.start)
                return 0;
}

int32_t insert_elem(ringbuffer * r, void * new_elem)
{
        return insert_bulk(r, new_elem, 1);
}
