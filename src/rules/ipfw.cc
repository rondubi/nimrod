#include "ipfw.hh"
#include "pattern.hh"
#include "proto/ipv4.hh"

#include <cassert>
#include <cstdio>
#include <cstdlib>

// Some site with the IPFW BNF: http://info.iet.unipi.it/~luigi/ip_dummynet/original.html

namespace nimrod
{
constexpr int RULE_TABLE_CAPACITY = ((1 << 16) - 1);

int Rules::add(
        int32_t rule_number,
        action action,
        Expr * to_expr,
        Expr * from_expr,
        std::optional<PacketHandler> handler)
{
        assert(RULE_TABLE_CAPACITY > rule_number);

        rule_table[rule_number] = Rules::Rule{
                .act = action,
                .to_expr = std::unique_ptr<Expr>(to_expr),
                .from_expr = std::unique_ptr<Expr>(from_expr),
                .handler = handler,
        };
        return 0;
}
int Rules::add(
        int32_t rule_number,
        action action,
        ipv4_addr to,
        ipv4_addr from,
        std::optional<PacketHandler> handler)
{
        return add(
                rule_number,
                action,
                new ExactMatch(to),
                new ExactMatch(from),
                handler);
}

int Rules::apply_rules(packet && packet) const
{
        for (const auto & [rnum, rule] : rule_table)
        {
                if (rule.matches(packet.ipv4_header()))
                {
                        switch (rule.act)
                        {
                                case action::allow:
                                        return rule.handler.value_or(
                                                default_handle_fn)(
                                                std::move(packet));
                                case action::deny:
                                        return DENIED;
                                default:
                                        return FAIL;
                        }
                }
        }

        return FAIL;
}

}
