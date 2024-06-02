#include <iostream>
#include <memory>
#include <vector>

#include "core.hh"
#include "packet.hh"
#include "proto/ipv4.hh"
#include "queue.hh"
#include "receiver/limit.hh"
#include "receiver/repeat.hh"
#include "rules/ipfw.hh"
#include "rules/pattern.hh"
#include "sender/link.hh"
#include "sender/log.hh"
#include "sender/time.hh"

using namespace std::chrono_literals;

void fuse(nimrod::receiver & r, nimrod::sender & s);

namespace nimrod
{
struct blackhole : sender
{
        send_result send(packet && p) { return send_result::ok; }
};
}

int main()
{
        // Assuming just one flow
        auto get_next_packet = [&]()
        {
                static int i = 0;
                return nimrod::ipv4_packet{
                    .header = {
                        .src = {std::vector{0u, 1u, 100u}[i++ % 3]},
                        .dst = {0},
                        .total_length = 32,
                    },
                    .payload = std::vector<std::byte>(8, std::byte{}) // models UDP header
                };
        };

        auto stage_d = std::make_shared<nimrod::logger>();
        auto stage_c_r = std::make_shared<nimrod::link>(stage_d, 0ns, 1024);
        auto stage_c_d = std::make_shared<nimrod::link>(
                stage_c_r, 20ms, std::numeric_limits<size_t>::max());
        auto stage_c_0 = std::make_shared<nimrod::link>(
                stage_c_d, 0ns, std::numeric_limits<size_t>::max());
        auto stage_b_100 = std::make_shared<nimrod::RulesSender>(stage_c_0);
        for (int i = 2; i <= 100; ++i)
        {
                stage_b_100->rule_table.add(
                        i,
                        nimrod::action::allow,
                        new nimrod::And(
                                new nimrod::Not(new nimrod::ExactMatch({1})),
                                new nimrod::ExactMatch({100})),
                        new nimrod::ExactMatch({0}));
        }
        auto stage_b = std::make_shared<nimrod::RulesSender>(stage_b_100);
        stage_b->rule_table.add(
                1,
                nimrod::action::allow,
                new nimrod::Not(new nimrod::ExactMatch({1})),
                new nimrod::ExactMatch({0}));

        auto start_time = std::chrono::high_resolution_clock::now();

        while (true)
        {
                auto pkt = get_next_packet();
                // TODO: figure out a better way to drop packets with src 0
                if (pkt.header.src == nimrod::ipv4_addr{0})
                        continue;
                auto elapsed_time
                        = std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::high_resolution_clock::now()
                                - start_time);
                if (elapsed_time >= 1500ms)
                        break;

                stage_b->send(pkt);
        }

        return 0;
}

void fuse(nimrod::receiver & r, nimrod::sender & s)
{
        nimrod::packet packet;

        while (true)
        {
                switch (r.recv(&packet))
                {
                        case nimrod::recv_result::closed:
                                return;
                        case nimrod::recv_result::ok: {
                                s.send(std::move(packet));
                                break;
                        }
                }
        }
}
