#include <cstdio>
#include <cstdlib>
#include <functional>
#include <map>
#include <optional>

#include "core.hh"
#include "packet.hh"
#include "pattern.hh"
#include "proto/ipv4.hh"
#include "sender.hh"

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

using PacketHandler = std::function<int(packet &&)>;

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
                ipv4_addr ipv4_to,
                ipv4_addr ipv4_from,
                std::optional<PacketHandler> handler = {});
        int add(int rule_number,
                action action,
                Expr * to_expr,
                Expr * from_expr,
                std::optional<PacketHandler> handler = {});

        // NOTE: should return status
        int apply_rules(packet && packet) const;

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
                        return to_expr->match(p.dst) && from_expr->match(p.src);
                }
        };

        std::map<int, Rule> rule_table;
        PacketHandler default_handle_fn;
};

PacketHandler get_handler(std::shared_ptr<sender> s)
{
        return [&](packet && p)
        { return s->send(std::move(p)) == send_result::ok ? 0 : FAIL; };
}

struct RulesSender : public sender
{
        RulesSender(std::shared_ptr<sender> s_)
            : s(s_), rule_table(get_handler(s_))
        {
        }

        send_result send(packet && p) override
        {
                switch (rule_table.apply_rules(std::move(p)))
                {
                        case DENIED:
                                return send_result::closed;
                        case FAIL:
                                return send_result::closed;
                        default:
                                return send_result::ok;
                }
        }

        std::shared_ptr<sender> s;

        Rules rule_table;
};

}
