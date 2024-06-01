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