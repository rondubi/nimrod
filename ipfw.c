#include "ipfw.h"

#include <assert.h>
#include <stdlib.h>

// Some site with the IPFW BNF: http://info.iet.unipi.it/~luigi/ip_dummynet/original.html

// NOTE: assume for now that all protocols are IPv4
// NOTE: assume for now that rules are not probabilistic
// NOTE: assume rules are only on dummynet

// TODO: do we need to know what pipes are valid?
struct rule
{
        // NOTE: no pattern matching at the moment
        int32_t ipv4_from;
        int32_t ipv4_to;
        enum action_t action;
};

#define RULE_TABLE_CAPACITY (1 << 16  - 1)
static struct rule * rule_table;

void rules_init()
{
        rule_table = malloc(sizeof(struct rule) * RULE_TABLE_CAPACITY);
        memset(rule_table, -1, RULE_TABLE_CAPACITY);
}

int rule_add(int32_t rule_number, enum action_t action, int32_t ipv4_from, int32_t ipv4_to)
{
        assert(RULE_TABLE_CAPACITY > rule_number);;

        rule_table[rule_number] = (struct rule) {.action = action, .ipv4_from = ipv4_from, .ipv4_to = ipv4_to, };

        return 0;
}

// NOTE: return 0 if no match, return status otherwise
int try_match(packet_t packet, int i)
{
        if (!(packet.ipv4_from == rule_table[i]->ipv4_from && packet.ipv4_to == rule_table[i]->ipv4_to))
                return 0;

        return perform_action(packet, i);
}

int uninitialized_rule(int i)
{
        return rule_table[i]->ipv4_from == -1 && rule_table[i]->ipv4_to == -1;
}

int rule_apply(packet_t packet)
{
        for (int i = 0; i < RULE_TABLE_CAPACITY; ++i)
        {
                if (uninitialized_rule(i))
                {
                        continue;
                }
                
                const int result = try_match(packet, i);
                if (result != 0)
                        return result;
        }

        return default_action(packet);
}

