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

/*
int main()
{
        auto repeat = std::make_shared<nimrod::repeat>(nimrod::ipv4_packet{
            .header = {
                .total_length = 64,
            },
            .payload = std::vector<std::byte>(40, std::byte{})
        });

        auto limited_repeat = std::make_shared<nimrod::limit>(16, repeat);

        nimrod::queue queue{5};

        std::shared_ptr<nimrod::sender> send;

        send = std::make_shared<nimrod::logger>();
        // send = std::make_shared<nimrod::fixed_delay>(10ms, std::move(send));
        send = std::make_shared<nimrod::link>(send, 10ms, 1024);
        send = std::make_shared<nimrod::timer>(
                "link 1",
                [](auto name, const auto & packet, auto duration) {
                        std::cout << name << " took " << duration
                                  << " to send packet" << "\n";
                },
                send);

        fuse(*limited_repeat, *send);

        // std::this_thread::sleep_for(1s);
}
*/

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

        auto stage_d = std::make_shared<nimrod::blackhole>();
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

        // TODO: figure out a way to drop packets with src 0
        auto start_time = std::chrono::high_resolution_clock::now();

        while (true)
        {
                auto pkt = get_next_packet();
                if (pkt.header.src == nimrod::ipv4_addr{0})
                        continue;
                auto elapsed_time
                        = std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::high_resolution_clock::now()
                                - start_time);
                if (elapsed_time >= std::chrono::milliseconds(1500))
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
