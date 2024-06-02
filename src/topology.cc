#include <memory>
#include <vector>

#include "core.hh"
#include "fuse.hh"
#include "queue.hh"
#include "receiver/round_robin.hh"
#include "rules/ipfw.hh"
#include "sender/black_hole.hh"
#include "sender/link.hh"
#include "topology.hh"

namespace nimrod
{
std::vector<std::shared_ptr<sender>> case_a(std::size_t flows)
{
        std::vector<std::shared_ptr<sender>> senders;

        for (size_t i = 0; i < flows; ++i)
                senders.push_back(std::make_shared<black_hole>());

        return senders;
}

std::vector<std::shared_ptr<sender>>
case_b(std::size_t flows, std::size_t rules)
{
        std::shared_ptr<sender> s;

        s = std::make_shared<black_hole>();


        auto rules_sender = std::make_shared<RulesSender>(s);
        s = rules_sender;

        for (int i = 1; i < rules + 1; ++i)
                rules_sender->rule_table.add(i, action::allow, {1}, {2});

        rules_sender->rule_table.add(rules + 1, action::deny, {1}, {3});

        auto scheduler = std::make_shared<round_robin>();

        std::vector<std::shared_ptr<sender>> senders;
        for (size_t i = 0; i < flows; ++i)
        {
                auto q = std::make_shared<queue>(64);
                scheduler->add_receiver(q);
                senders.push_back(std::move(q));
        }

        fuse(scheduler, s);
        return senders;
}

std::vector<std::shared_ptr<sender>>
case_c(std::size_t flows, std::chrono::nanoseconds delay, std::size_t bandwidth)
{
        std::shared_ptr<sender> s;

        s = std::make_shared<black_hole>();
        s = std::make_shared<link>(s, delay, bandwidth);

        auto scheduler = std::make_shared<round_robin>();

        std::vector<std::shared_ptr<sender>> senders;
        for (size_t i = 0; i < flows; ++i)
        {
                auto q = std::make_shared<queue>(64);
                scheduler->add_receiver(q);
                senders.push_back(std::move(q));
        }

        fuse(scheduler, s);
        return senders;
}

// std::vector<std::shared_ptr<sender>> case_b(std::size_t flows)
// {
//         std::vector<std::shared_ptr<sender>> senders;


//         std::shared_ptr<sender> s;

//         s = std::make_shared<black_hole>();
//         s = std::make_shared<RulesSender>();

//         for (size_t i = 0; i < flows; ++i)
//                 senders.push_back(std::make_shared<black_hole>());

//         return senders;
// }
}