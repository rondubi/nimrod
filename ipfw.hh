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

        // returns status
        // NOTE: does not prevent overriding of existing rules
        int add(int rule_number, action action, int32_t ipv4_from, int32_t ipv4_to);

        // NOTE: should return status
        int apply_rules(packet packet) const;

private:
        // NOTE: assume for now that all protocols are IPv4
        // NOTE: assume for now that rules are not probabilistic
        // NOTE: assume rules are only on dummynet

        struct Rule
        {
                action action;
                std::function<bool(packet)> matches;
        };

        std::map<int, Rule> rule_table;
        std::function<int(packet)> packet_send_fn;
};

