#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "the_one_ring.h"

void test_push_pop() {
        ringbuffer r = {
                .storage = malloc(3 * sizeof(void *)),
                .capacity = 3,
                .start = 0,
                .end = 0,
        };

        int a = 1, b = 2, c = 3;

        rb_push_elem(&r, &a);
        rb_push_elem(&r, &b);
        rb_push_elem(&r, &c);

        int * popped1 = (int *)rb_pop_elem(&r);
        int * popped2 = (int *)rb_pop_elem(&r);
        int * popped3 = (int *)rb_pop_elem(&r);

        assert(*popped1 == 1);
        assert(*popped2 == 2);
        assert(*popped3 == 3);

        free(r.storage);
}

int main()
{
        test_push_pop();
        printf("Basic push-pop succeeded!\n");
        return 0;
}

