#include "ipfw.hh"

#include <cassert>
#include <cstdlib>

int pipe_send_packet(packet packet);

// Some site with the IPFW BNF: http://info.iet.unipi.it/~luigi/ip_dummynet/original.html

#define RULE_TABLE_CAPACITY (1 << 16  - 1)

int Rules::add(int32_t rule_number, action action, int32_t ipv4_from, int32_t ipv4_to)
{
        assert(RULE_TABLE_CAPACITY > rule_number);;

        rule_table[rule_number] = Rules::RUle {.action = action, .ipv4_from = ipv4_from, .ipv4_to = ipv4_to, };

        return 0;
}

#define FAIL -5 // ????

int Rules::apply_rules(packet packet) const
{
        for (const auto & [rnum, rule] : rules_table)
        {
                if (rule.ipv4_from == packet.ipv4_from && rule.ipv4_to == packet.ipv4_to)
                {
                        // TODO: handle other actions
                        return pipe_send_packet(packet);
                }
        }

        return FAIL;
}

