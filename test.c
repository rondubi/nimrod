#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "the_one_ring.h"

void test_push_pop()
{
        ringbuffer r;

        rb_create(&r, 3);

        int a = 1, b = 2, c = 3;

        assert(rb_push_elem(&r, &a));
        assert(rb_push_elem(&r, &b));
        assert(rb_push_elem(&r, &c));

        int * popped1 = (int *)rb_pop_elem(&r);
        int * popped2 = (int *)rb_pop_elem(&r);
        int * popped3 = (int *)rb_pop_elem(&r);

        assert(*popped1 == 1);
        assert(*popped2 == 2);
        assert(*popped3 == 3);

        rb_cleanup(&r);
}

void test_empty_pop()
{
        ringbuffer r;
        rb_create(&r, 5);
        assert(rb_pop_elem(&r) == NULL);
        rb_cleanup(&r);
}

// TODO: don't push on full ringbuffer
// TODO: bulk tests

int main()
{
        test_push_pop();
        printf("Basic push-pop test succeeded!\n");

        test_empty_pop();
        printf("Empty pop test succeeded!\n");
        return 0;
}

