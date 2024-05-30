#include "ipfw.hh"

#include <cassert>
#include <iostream>

bool test_basic_rule_add()
{
        int status = 0;

        Rules r([&](packet p){ status = 13; return 0; });

        r.add(1, action::ALLOW, 1, 1);
        int res = r.apply_rules({1, 1});

        return res == 0 && status == 13;
}

int main()
{
        assert(test_basic_rule_add());
        printf("Finished basic test!\n");

        return 0;
}

