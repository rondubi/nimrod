#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <string_view>
#include <thread>
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
#include "topology.hh"

using namespace std::chrono_literals;

void fuse(nimrod::receiver & r, nimrod::sender & s);


template <typename F>
void run_test_case(
        std::string_view name,
        std::vector<std::shared_ptr<nimrod::sender>> topology,
        std::chrono::nanoseconds delay,
        F gen);

int main()
{
        std::random_device random;
        // Assuming just one flow
        auto get_next_packet = [&random]()
        {
                return nimrod::ipv4_packet{
                    .header = {
                        .id = static_cast<uint16_t>(random()),
                        .src = {1},
                        .dst = {3},
                       
                    },
                    .payload = {std::vector<std::byte>{8, std::byte{}}} // models UDP header
                };
        };

        auto top = nimrod::case_a(1);

        const std::size_t bw_limit = 1024 * 1024 * 1024;

        run_test_case("case a", nimrod::case_a(1), {}, get_next_packet);
        run_test_case("case b", nimrod::case_b(1, 1), {}, get_next_packet);
        run_test_case(
                "case b_100", nimrod::case_b(1, 100), {}, get_next_packet);

        run_test_case(
                "case c0",
                nimrod::case_c(
                        1,
                        std::chrono::milliseconds{0},
                        std::numeric_limits<std::size_t>::max()),
                {},
                get_next_packet);

        run_test_case(
                "case cd",
                nimrod::case_c(
                        1,
                        std::chrono::milliseconds{20},
                        std::numeric_limits<std::size_t>::max()),
                std::chrono::milliseconds{20},
                get_next_packet);


        run_test_case(
                "case cr",
                nimrod::case_c(1, std::chrono::milliseconds{0}, bw_limit),
                {},
                get_next_packet);


        run_test_case("case a'", nimrod::case_a(1000), {}, get_next_packet);
        run_test_case("case b'", nimrod::case_b(1000, 1), {}, get_next_packet);
        run_test_case(
                "case c'",
                nimrod::case_c(1000, std::chrono::milliseconds{0}, bw_limit),
                {},
                get_next_packet);

        // std::uint64_t sent = 0;
        // const auto time = std::chrono::seconds{1};
        // const auto start = std::chrono::steady_clock::now();
        // const auto end = start + time;

        // while (std::chrono::steady_clock::now() < end)
        //         for (auto & s : top)
        //         {
        //                 s->send(get_next_packet());
        //                 sent++;
        //         }

        // std::cout << "processed " << sent << " packets in " << time;

        // auto stage_d = std::make_shared<nimrod::logger>();
        // auto stage_c_r = std::make_shared<nimrod::link>(stage_d, 0ns, 1024);
        // auto stage_c_d = std::make_shared<nimrod::link>(
        //         stage_c_r, 20ms, std::numeric_limits<size_t>::max());
        // auto stage_c_0 = std::make_shared<nimrod::link>(
        //         stage_c_d, 0ns, std::numeric_limits<size_t>::max());
        // auto stage_b_100 = std::make_shared<nimrod::RulesSender>(stage_c_0);
        // for (int i = 2; i <= 100; ++i)
        // {
        //         stage_b_100->rule_table.add(
        //                 i,
        //                 nimrod::action::allow,
        //                 new nimrod::And(
        //                         new nimrod::Not(new nimrod::ExactMatch({1})),
        //                         new nimrod::ExactMatch({100})),
        //                 new nimrod::ExactMatch({0}));
        // }
        // auto stage_b = std::make_shared<nimrod::RulesSender>(stage_b_100);
        // stage_b->rule_table.add(
        //         1,
        //         nimrod::action::allow,
        //         new nimrod::Not(new nimrod::ExactMatch({1})),
        //         new nimrod::ExactMatch({0}));

        // auto start_time = std::chrono::high_resolution_clock::now();

        // while (true)
        // {
        //         auto pkt = get_next_packet();
        //         // TODO: figure out a better way to drop packets with src 0
        //         if (pkt.header.dst == nimrod::ipv4_addr{0})
        //                 continue;
        //         auto elapsed_time
        //                 = std::chrono::duration_cast<std::chrono::milliseconds>(
        //                         std::chrono::high_resolution_clock::now()
        //                         - start_time);
        //         if (elapsed_time >= 1500ms)
        //                 break;

        //         stage_b->send(pkt);
        // }

        return 0;
}


template <typename F>
void run_test_case(
        std::string_view name,
        std::vector<std::shared_ptr<nimrod::sender>> topology,
        std::chrono::nanoseconds delay,
        F gen)
{
        const std::uint64_t total_samples = 100'000 / topology.size();
        std::vector<std::chrono::nanoseconds> samples;

        nimrod::packet packet;

        const auto batch_size = 1'000;

        const auto sampling_start = std::chrono::steady_clock::now();
        std::chrono::seconds sample_time{1};

        std::uint64_t total_sent = 0;

        while (sampling_start + sample_time > std::chrono::steady_clock::now())
        {
                const auto start = std::chrono::steady_clock::now();
                for (size_t i = 0; i < batch_size; ++i)
                {
                        for (auto & s : topology)
                        {
                                s->send(gen());
                                total_sent++;
                        }
                }

                const auto end = std::chrono::steady_clock::now();
                samples.push_back((end - start) / batch_size);
        }
        std::this_thread::sleep_for(delay);
        const auto sampling_end = std::chrono::steady_clock::now();


        std::chrono::steady_clock::duration average;

        for (auto sample : samples)
                average += sample;

        average /= (samples.size());

        auto avg = (sampling_end - sampling_start) / total_sent;

        std::cout << "test '" << name << "': collected " << samples.size()
                  << " samples in " << sampling_end - sampling_start
                  << " with an average time of " << average << " (" << avg
                  << ")" << "\n ";
}
// void fuse(nimrod::receiver & r, nimrod::sender & s)
// {
//         nimrod::packet packet;

//         while (true)
//         {
//                 switch (r.recv(&packet))
//                 {
//                         case nimrod::recv_result::closed:
//                                 return;
//                         case nimrod::recv_result::ok: {
//                                 s.send(std::move(packet));
//                                 break;
//                         }
//                 }
//         }
// }
