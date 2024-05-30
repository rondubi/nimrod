#include <cstdlib>
#include <map>
#include <functional>

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

// TODO: status indicator enum
constexpr int FAIL = -5;
constexpr int DENIED = -3;


struct Rules
{
        Rules(std::function<int(packet)> fn) : rule_table{}, packet_send_fn{fn} {}

        // NOTE: should return status
        // NOTE: does not prevent overriding of existing rules
        int add(int rule_number, action action, int32_t ipv4_from, int32_t ipv4_to);

        // NOTE: should return status
        // TODO: wtf is a packet_t
        int apply_rules(packet packet) const;
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

        std::map<int, Rule> rule_table;
        std::function<int(packet)> packet_send_fn;
};

