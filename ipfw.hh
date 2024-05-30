#include <cstdlib>
#include <map>

enum action
{
        ALLOW,
        DENY
        // TODO: more actions
};

struct packet
{
        int32_t ipv4_from;
        int32_t ipv4_to;
};

struct Rules
{
        Rules() : rules_table();

        // NOTE: should return status
        // NOTE: does not prevent overriding of existing rules
        int rule_add(int rule_number, action_t action, int32_t ipv4_from, int32_t ipv4_to);

        // NOTE: should return status
        // TODO: wtf is a packet_t
        int rule_apply(packet_t packet);
        // TODO: pipe and queue configuration

private:
        // NOTE: assume for now that all protocols are IPv4
        // NOTE: assume for now that rules are not probabilistic
        // NOTE: assume rules are only on dummynet

        // TODO: do we need to know what pipes are valid?
        struct Rule
        {
                // NOTE: no pattern matching at the moment
                int32_t ipv4_from;
                int32_t ipv4_to;
                action action;

                // TODO: matches func and regex support
                // bool matches(packet p) const;
        };

        std::map<Rule> rules_table;
};

