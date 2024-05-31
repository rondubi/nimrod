#include "ipfw.hh"
#include "pattern.hh"

#include <cassert>
#include <cstdlib>

// Some site with the IPFW BNF: http://info.iet.unipi.it/~luigi/ip_dummynet/original.html

constexpr int RULE_TABLE_CAPACITY = ((1 << 16)  - 1);

int Rules::add(int32_t rule_number, action action, int32_t ipv4_from, int32_t ipv4_to)
{
        assert(RULE_TABLE_CAPACITY > rule_number);;
        Rules::Rule r {
                .act = action,
                .to_expr = std::make_unique<ExactMatch>(ipv4_to),
                .from_expr = std::make_unique<ExactMatch>(ipv4_from),
        };

        // rule_table.insert({rule_number, Rules::Rule{action, new ExactMatch(ipv4_to), new ExactMatch(ipv4_from)}});
        rule_table[rule_number] = std::move(r);
        return 0;
}

int Rules::apply_rules(packet packet) const
{
        for (const auto & [rnum, rule] : rule_table)
        {
                if (rule.matches(packet))
                {
                        switch (rule.act)
                        {
                                case ALLOW:
                                        return packet_send_fn(packet);
                                case DENY:
                                        return DENIED;
                                default:
                                        return FAIL;
                        }
                }
        }

        return FAIL;
}

