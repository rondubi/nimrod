#include "ipfw.h"

#include <cassert>
#include <cstdlib>

int pipe_send_packet(packet packet);

// Some site with the IPFW BNF: http://info.iet.unipi.it/~luigi/ip_dummynet/original.html

#define RULE_TABLE_CAPACITY (1 << 16  - 1)

int Rules::rule_add(int32_t rule_number, action action, int32_t ipv4_from, int32_t ipv4_to)
{
        assert(RULE_TABLE_CAPACITY > rule_number);;

        rule_table[rule_number] = Rules::RUle {.action = action, .ipv4_from = ipv4_from, .ipv4_to = ipv4_to, };

        return 0;
}

// NOTE: return 0 if no match, return status otherwise
int try_match(packet_t packet, int i)
{
        if (!(packet.ipv4_from == rule_table[i]->ipv4_from && packet.ipv4_to == rule_table[i]->ipv4_to))
                return 0;

        return perform_action(packet, i);
}

int Rules::rule_apply(packet packet) const
{
//        for (int i = 0; i < RULE_TABLE_CAPACITY; ++i)
//        {
//                if (is_uninitialized_rule(i))
//                {
//                        continue;
//                }
//                
//                const int result = try_match(packet, i);
//                if (result != 0)
//                        return result;
//        }
//
//        return default_action(packet);
        for (const auto & [rnum, rule] : rules_table)
        {
                if (rule.ipv4_from == packet.ipv4_from && rule.ipv4_to == packet.ipv4_to)
                {
                        // TODO: handle other actions
                        return pipe_send_packet(packet);
                }
        }
}

