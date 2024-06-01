#include <cstdlib>
#include <functional>
#include <map>
#include <optional>

#include "packet.hh"
#include "pattern.hh"
#include "proto/ipv4.hh"

namespace nimrod
{
enum class action
{
        allow,
        deny
        // TODO: more actions
};


// TODO: status indicator enum
constexpr int FAIL = -5;
constexpr int DENIED = -3;

using PacketHandler = std::function<int(const ipv4_packet_header &)>;

struct Rules
{
        Rules(PacketHandler default_fn)
            : rule_table(), default_handle_fn(default_fn)
        {
        }

        // returns status
        // NOTE: does not prevent overriding of existing rules
        int add(int rule_number,
                action action,
                int32_t ipv4_to,
                int32_t ipv4_from,
                std::optional<PacketHandler> handler = {});
        int add(int rule_number,
                action action,
                Expr * to_expr,
                Expr * from_expr,
                std::optional<PacketHandler> handler = {});

        // NOTE: should return status
        int apply_rules(const ipv4_packet_header & packet) const;

private:
        // NOTE: assume for now that all protocols are IPv4
        // NOTE: assume for now that rules are not probabilistic
        // NOTE: assume rules are only on dummynet

        struct Rule
        {
                action act;
                std::unique_ptr<Expr> to_expr;
                std::unique_ptr<Expr> from_expr;
                std::optional<PacketHandler> handler;

                bool matches(const ipv4_packet_header & p) const
                {
                        return to_expr->match(p.src.bits)
                                && from_expr->match(p.dst.bits);
                }
        };

        std::map<int, Rule> rule_table;
        PacketHandler default_handle_fn;
};

}