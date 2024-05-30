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

bool test_pri_in_order()
{
        int status = 0;

        Rules r([&](packet p){ status = 13; return 0; });

        r.add(1, action::ALLOW, 1, 1);
        r.add(2, action::DENY, 1, 1);
        int res = r.apply_rules({1, 1});

        return res == 0 && status == 13;
}

bool test_pri_out_of_order()
{
        int status = 0;

        Rules r([&](packet p){ status = 13; return 0; });

        r.add(2, action::DENY, 1, 1);
        r.add(1, action::ALLOW, 1, 1);
        int res = r.apply_rules({1, 1});

        return res == 0 && status == 13;
}

int main()
{
        assert(test_basic_allow_rule());
        printf("Finished basic allow test!\n");

        assert(test_basic_deny_rule());
        printf("Finished basic deny test!\n");

        assert(test_pri_in_order());
        printf("Finished priority in order test!\n");

        assert(test_pri_out_of_order());
        printf("Finished priority out of order test!\n");

        return 0;
}

