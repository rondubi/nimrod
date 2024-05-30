#include "ipfw.hh"

#include <cassert>
#include <iostream>

bool test_basic_allow_rule()
{
        int status = 0;

        Rules r([&](packet p){ status = 13; return 0; });

        r.add(1, action::ALLOW, 1, 1);
        int res = r.apply_rules({1, 1});

        return res == 0 && status == 13;
}

bool test_basic_deny_rule()
{
        int status = 0;

        Rules r([&](packet p){ status = 13; return 0; });

        r.add(1, action::DENY, 1, 1);
        int res = r.apply_rules({1, 1});

        return res == DENIED && status == 0;
}

int main()
{
        assert(test_basic_allow_rule());
        printf("Finished basic allow test!\n");

        assert(test_basic_deny_rule());
        printf("Finished basic deny test!\n");

        return 0;
}

