#include "ipfw.hh"

#include <cassert>
#include <iostream>

using namespace nimrod;

namespace
{
bool test_basic_allow_rule()
{
        int status = 0;

        Rules r(
                [&](packet p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1, action::ALLOW, 1, 1);
        int res = r.apply_rules({1, 1});

        return res == 0 && status == 13;
}

bool test_basic_deny_rule()
{
        int status = 0;

        Rules r(
                [&](packet p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1, action::DENY, 1, 1);
        int res = r.apply_rules({1, 1});

        return res == DENIED && status == 0;
}

bool test_pri_in_order()
{
        int status = 0;

        Rules r(
                [&](packet p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1, action::ALLOW, 1, 1);
        r.add(2, action::DENY, 1, 1);
        int res = r.apply_rules({1, 1});

        return res == 0 && status == 13;
}

bool test_pri_out_of_order()
{
        int status = 0;

        Rules r(
                [&](packet p)
                {
                        status = 13;
                        return 0;
                });

        r.add(2, action::DENY, 1, 1);
        r.add(1, action::ALLOW, 1, 1);
        int res = r.apply_rules({1, 1});

        return res == 0 && status == 13;
}

bool test_pattern_match_or()
{
        int status = 0;

        Rules r(
                [&](packet p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1,
              action::ALLOW,
              new Or(new ExactMatch(1), new ExactMatch(2)),
              new ExactMatch(1));
        const int res1 = r.apply_rules({1, 1});
        const int status1 = status;
        const int res2 = r.apply_rules({2, 1});
        const int status2 = status;
        status = 0;
        const int res3 = r.apply_rules({3, 1});

        return res1 == 0 && status1 == 13 && res2 == 0 && status2 == 13
                && res3 == FAIL && status == 0;
}

bool test_pattern_match_not()
{
        int status = 0;

        Rules r(
                [&](packet p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1, action::DENY, new Not(new ExactMatch(1)), new ExactMatch(1));
        const int res1 = r.apply_rules({1, 1});
        const int status1 = status;
        const int res2 = r.apply_rules({2, 1});

        return res1 == FAIL && status1 == 0 && res2 == DENIED && status == 0;
}

bool test_pattern_match_and()
{
        int status = 0;

        Rules r(
                [&](packet p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1,
              action::ALLOW,
              new And(new Not(new ExactMatch(1)), new Not(new ExactMatch(2))),
              new ExactMatch(1));
        const int res1 = r.apply_rules({1, 1});
        const int status1 = status;
        const int res2 = r.apply_rules({2, 1});
        const int status2 = status;
        const int res3 = r.apply_rules({3, 1});

        return res1 == FAIL && status1 == 0 && res2 == FAIL && status2 == 0
                && res3 == 0 && status == 13;
}

bool test_pattern_match_any()
{
        int status = 0;

        Rules r(
                [&](packet p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1, action::ALLOW, new LengthMatch(0, 0), new ExactMatch(1));
        const int res1 = r.apply_rules({1, 1});
        const int status1 = status;
        status = 0;
        const int res2 = r.apply_rules({2, 1});
        const int status2 = status;
        status = 0;
        const int res3 = r.apply_rules({3, 1});

        return res1 == 0 && status1 == 13 && res2 == 0 && status2 == 13
                && res3 == 0 && status == 13;
}

bool test_pattern_match_length_match()
{
        int status = 0;

        Rules r(
                [&](packet p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1,
              action::ALLOW,
              new LengthMatch(0xffffffff, 3),
              new ExactMatch(1));
        const int res1 = r.apply_rules({0, 1});
        const int status1 = status;
        const int res2 = r.apply_rules({(int)0xff000000, 1});
        const int status2 = status;
        const int res3 = r.apply_rules({(int)0xffff0000, 1});
        const int status3 = status;
        const int res4 = r.apply_rules({(int)0xffffff00, 1});
        const int status4 = status;
        status = 0;
        const int res5 = r.apply_rules({(int)0xffffffff, 1});

        return res1 == FAIL && status1 == 0 && res2 == FAIL && status2 == 0
                && res3 == FAIL && status3 == 0 && res4 == 0 && status4 == 13
                && res5 == 0 && status == 13;
}

bool test_custom_handler()
{
        int status = 0;

        Rules r(
                [&](packet p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1,
              action::ALLOW,
              1,
              1,
              {[&](packet p)
               {
                       status = 14;
                       return 0;
               }});
        int res = r.apply_rules({1, 1});

        return res == 0 && status == 14;
}
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

        assert(test_pattern_match_or());
        printf("Finished pattern match OR test!\n");

        assert(test_pattern_match_not());
        printf("Finished pattern match NOT test!\n");

        assert(test_pattern_match_and());
        printf("Finished pattern match AND test!\n");

        assert(test_pattern_match_any());
        printf("Finished pattern match * test!\n");

        assert(test_pattern_match_length_match());
        printf("Finished pattern match length match test!\n");

        assert(test_custom_handler());
        printf("Finished custom handler test!\n");

        return 0;
}
