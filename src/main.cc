#include <iostream>
#include <memory>
#include <vector>

#include "core.hh"
#include "packet.hh"
#include "proto/ipv4.hh"
#include "queue.hh"
#include "receiver/limit.hh"
#include "receiver/repeat.hh"
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
        send_result send(packet && p)
        {
                return send_result::ok;
        }
};
}

int main()
{
        // Assuming just one flow

        Generator g;

        auto stage_d = std::make_shared<nimrod::blackhole>();
        auto stage_c_r = std::make_shared<nimrod::link>(d, 0, 1024);
        auto stage_c_d = std::make_shared<nimrod::link>(c_r, 20ms, std::numeric_limits<size_t>::max());
        auto stage_c_0 = std::make_shared<nimrod::link>(c_d, 0, std::numeric_limits<size_t>::max());
        auto stage_b_100 = std::make_shared<nimrod::rulesender>(stage_c_0->send);
        // TODO: add 99 rules to b_100
        auto stage_b = std::make_shared<nimrod::rulesender>(stage_b_100->send);
        // TODO: add a rule to b
        stage_b->

        int i = 0;
        auto get_next_packet = [&](){
                return nimrod::ipv4_packet{
                    .header = {
                        .src = {{0, 1, }[i++ % 3]},
                        .dst = {0},
                        .total_length = 32,
                    },
                    .payload = std::vector<std::byte>(8, std::byte{}) // models UDP header
                };
        };
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
