#include "ipfw.h"

#include <assert.h>

// Some site with the IPFW BNF: http://info.iet.unipi.it/~luigi/ip_dummynet/original.html

// NOTE: assume for now that all protocols are IPv4
// NOTE: assume for now that rules are not probabilistic
// NOTE: assume rules are only on dummynet

// TODO: do we need to know what pipes are valid?
struct rule
{
        int32_t ipv4_from;
        int32_t ipv4_to;
        enum action_t action;
};

struct rule_table_t
{
        int32_t capacity;
        int32_t numrules;
        struct rule * rules;
};

static struct rule_table_t rule_table;

void rules_init()
{
        rule_table.capacity = 10;
        rule_table.numrules = 0;
        rule_table.rules = malloc(sizeof(struct rule) * rule_table.capacity);
}

void resize_rule_table()
{
        rule_table.capacity *= 2;
        assert(rule_table.capacity <= (1 << 16));
        rule_table.rules = realloc(rule_table.rules, rule_table.capacity * sizeof(struct rule));
}

int rule_add(int32_t rule_number, enum action_t action, int32_t ipv4_from, int32_t ipv4_to)
{
        if (rule_table.capacity <= rule_number)
        {
                resize_rule_table();
        }

        rule_table.rules[rule_number] = (struct rule) {.action = action, .ipv4_from = ipv4_from, .ipv4_to = ipv4_to, };
        ++rule_table.numrules;

        return 0;
}

int rule_apply(packet_t packet)
{
}

int rule_config(int pipe_number, void * options)
{
}

