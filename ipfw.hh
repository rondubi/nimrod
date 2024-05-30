#include <stdlib.h>

enum action_t
{
        ALLOW,
        DENY
        // TODO: more actions
};

struct packet_t
{
        int32_t ipv4_from;
        int32_t ipv4_to;
};

void rules_init();

// NOTE: should return status
// NOTE: does not prevent overriding of existing rules
int rule_add(int rule_number, enum action_t action, int32_t ipv4_from, int32_t ipv4_to);

// NOTE: should return status
// TODO: wtf is a packet_t
int rule_apply(struct packet_t packet);

// TODO: pipe and queue configuration

