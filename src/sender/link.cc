#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <queue>
#include <ratio>
#include <thread>

#include "core.hh"
#include "link.hh"
#include "packet.hh"
#include "receiver/limit.hh"

namespace nimrod
{
using clock = std::chrono::system_clock;

struct in_flight
{
        clock::time_point arrival;
        packet packet;
};

struct link::shared
{
        std::shared_ptr<sender> next;
        std::chrono::nanoseconds propagation_delay;
        std::optional<bandwidth<double>> limit;

        clock::time_point prev_packet_done_sending = clock::now();
        std::queue<in_flight> packets;
        std::mutex mutex;
        std::condition_variable cond;
        std::atomic_bool done = false;
        std::thread worker;

        std::chrono::nanoseconds send_time(const packet & p) const
        {
                if (!limit.has_value())
                        return {};

                double seconds = p.total_size() / limit->count;
                std::chrono::duration<double> s{seconds};
                return std::chrono::duration_cast<std::chrono::nanoseconds>(s);
                // return std::chrono::nanoseconds{
                // p.total_size() / bytes_per_second * std::nano::den};
        }
};


link::~link()
{
        shared_state_->done = true;
        shared_state_->cond.notify_one();
        shared_state_->worker.join();
}

link::link(
        std::shared_ptr<sender> next,
        std::chrono::nanoseconds propagation_delay,
        std::optional<bandwidth<double>> limit)
    : shared_state_{new shared{next, propagation_delay, limit}}
{
        shared_state_->worker = std::thread{[shared = shared_state_]
                                            { worker(std::move(shared)); }};
}


send_result link::send(packet && p)
{
        p.passed_along();

        auto & shared = *shared_state_;

        const auto send_time = shared.send_time(p);

        auto start = clock::now();

        if (start < shared.prev_packet_done_sending)
                start = shared.prev_packet_done_sending;

        shared.prev_packet_done_sending
                = std::chrono::time_point_cast<clock::time_point::duration>(
                        start + send_time);

        const auto arrival = start + send_time + shared.propagation_delay;

        std::unique_lock lock{shared_state_->mutex};

        shared.packets.push(in_flight{
                std::chrono::time_point_cast<clock::time_point::duration>(
                        arrival),
                std::move(p)});
        lock.unlock();

        if (send_time != std::chrono::seconds{0})
                std::this_thread::sleep_until(start + send_time);

        shared.cond.notify_one();

        return send_result::ok;
}

void link::worker(std::shared_ptr<shared> shared)
{
        std::unique_lock lock{shared->mutex};

        for (;;)
        {
                if (shared->packets.empty())
                {
                        if (shared->done)
                                return;

                        shared->cond.wait(lock);
                        continue;
                }

                auto & p = shared->packets.front();

                if (p.arrival < clock::now())
                {
                        shared->next->send(std::move(p.packet));
                        shared->packets.pop();
                        continue;
                }

                auto arrival = p.arrival;
                lock.unlock();
                std::this_thread::sleep_until(arrival);
                lock.lock();
        }
}
}