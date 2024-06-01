#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "proto/ipv4.hh"
#include "rules/ipfw.hh"

#include <cassert>
#include <cstdint>

using namespace nimrod;

ipv4_packet_header make_packet(uint32_t dst, uint32_t src)
{
        return {
                .src = {src},
                .dst = {dst},
        };
}


TEST_CASE("test_basic_allow_rule")
{
        int status = 0;

        Rules r(
                [&](const ipv4_packet_header & p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1, action::allow, ipv4_addr{1}, ipv4_addr{1});
        int res = r.apply_rules(make_packet(1, 1));

        CHECK(res == 0);
        CHECK(status == 13);
}

TEST_CASE("test_basic_deny_rule")
{
        int status = 0;

        Rules r(
                [&](const ipv4_packet_header & p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1, action::deny, ipv4_addr{1}, ipv4_addr{1});
        int res = r.apply_rules(make_packet(1, 1));

        CHECK(res == DENIED);
        CHECK(status == 0);
}

TEST_CASE("test_pri_in_order")
{
        int status = 0;

        Rules r(
                [&](const ipv4_packet_header & p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1, action::allow, ipv4_addr{1}, ipv4_addr{1});
        r.add(2, action::deny, ipv4_addr{1}, ipv4_addr{1});
        int res = r.apply_rules(make_packet(1, 1));

        CHECK(res == 0);
        CHECK(status == 13);
}

TEST_CASE("test_pri_out_of_order")
{
        int status = 0;

        Rules r(
                [&](const ipv4_packet_header & p)
                {
                        status = 13;
                        return 0;
                });

        r.add(2, action::deny, ipv4_addr{1}, ipv4_addr{1});
        r.add(1, action::allow, ipv4_addr{1}, ipv4_addr{1});
        int res = r.apply_rules(make_packet(1, 1));

        CHECK(res == 0);
        CHECK(status == 13);
}

TEST_CASE("test_pattern_match_or")
{
        int status = 0;

        Rules r(
                [&](const ipv4_packet_header & p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1,
              action::allow,
              new Or(new ExactMatch(ipv4_addr{1}),
                     new ExactMatch(ipv4_addr{2})),
              new ExactMatch(ipv4_addr{1}));
        const int res1 = r.apply_rules(make_packet(1, 1));
        const int status1 = status;
        const int res2 = r.apply_rules(make_packet(2, 1));
        const int status2 = status;
        status = 0;
        const int res3 = r.apply_rules(make_packet(3, 1));

        CHECK(res1 == 0);
        CHECK(status1 == 13);
        CHECK(res2 == 0);
        CHECK(status2 == 13);
        CHECK(res3 == FAIL);
        CHECK(status == 0);
}

TEST_CASE("test_pattern_match_not")
{
        int status = 0;

        Rules r(
                [&](const ipv4_packet_header & p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1,
              action::deny,
              new Not(new ExactMatch(ipv4_addr{1})),
              new ExactMatch(ipv4_addr{1}));
        const int res1 = r.apply_rules(make_packet(1, 1));
        const int status1 = status;
        const int res2 = r.apply_rules(make_packet(2, 1));

        CHECK(res1 == FAIL);
        CHECK(status1 == 0);
        CHECK(res2 == DENIED);
        CHECK(status == 0);
}

TEST_CASE("test_pattern_match_and")
{
        int status = 0;

        Rules r(
                [&](const ipv4_packet_header & p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1,
              action::allow,
              new And(new Not(new ExactMatch(ipv4_addr{1})),
                      new Not(new ExactMatch(ipv4_addr{2}))),
              new ExactMatch(ipv4_addr{1}));
        const int res1 = r.apply_rules(make_packet(1, 1));
        const int status1 = status;
        const int res2 = r.apply_rules(make_packet(2, 1));
        const int status2 = status;
        const int res3 = r.apply_rules(make_packet(3, 1));

        CHECK(res1 == FAIL);
        CHECK(status1 == 0);
        CHECK(res2 == FAIL);
        CHECK(status2 == 0);
        CHECK(res3 == 0);
        CHECK(status == 13);
}

TEST_CASE("test_pattern_match_any")
{
        int status = 0;

        Rules r(
                [&](const ipv4_packet_header & p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1,
              action::allow,
              new LengthMatch(ipv4_addr{0}, 0),
              new ExactMatch(ipv4_addr{1}));
        const int res1 = r.apply_rules(make_packet(1, 1));
        const int status1 = status;
        status = 0;
        const int res2 = r.apply_rules(make_packet(2, 1));
        const int status2 = status;
        status = 0;
        const int res3 = r.apply_rules(make_packet(3, 1));

        CHECK(res1 == 0);
        CHECK(status1 == 13);
        CHECK(res2 == 0);
        CHECK(status2 == 13);
        CHECK(res3 == 0);
        CHECK(status == 13);
}

TEST_CASE("test_pattern_match_length_match")
{
        int status = 0;

        Rules r(
                [&](const ipv4_packet_header & p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1,
              action::allow,
              new LengthMatch(ipv4_addr{0xffffffff}, 3),
              new ExactMatch(ipv4_addr{1}));
        const int res1 = r.apply_rules(make_packet(0, 1));
        const int status1 = status;
        const int res2 = r.apply_rules(make_packet(0xff000000, 1));
        const int status2 = status;
        const int res3 = r.apply_rules(make_packet(0xffff0000, 1));
        const int status3 = status;
        const int res4 = r.apply_rules(make_packet(0xffffff00, 1));
        const int status4 = status;
        status = 0;
        const int res5 = r.apply_rules(make_packet(0xffffffff, 1));

        CHECK(res1 == FAIL);
        CHECK(status1 == 0);
        CHECK(res2 == FAIL);
        CHECK(status2 == 0);
        CHECK(res3 == FAIL);
        CHECK(status3 == 0);
        CHECK(res4 == 0);
        CHECK(status4 == 13);
        CHECK(res5 == 0);
        CHECK(status == 13);
}

TEST_CASE("custom handler")
{
        int status = 0;

        Rules r(
                [&](const ipv4_packet_header & p)
                {
                        status = 13;
                        return 0;
                });

        r.add(1,
              action::allow,
              {1},
              {1},
              {[&](const ipv4_packet_header & p)
               {
                       status = 14;
                       return 0;
               }});
        int res = r.apply_rules(make_packet(1, 1));

        CHECK(res == 0);
        CHECK(status == 14);
}
