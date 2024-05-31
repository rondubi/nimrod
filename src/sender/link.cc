#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <memory>
#include <mutex>
#include <queue>
#include <ratio>
#include <thread>

#include "core.hh"
#include "link.hh"
#include "packet.hh"

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
        std::size_t bytes_per_second;

        clock::time_point prev_packet_done_sending = clock::now();
        std::queue<in_flight> packets;
        std::mutex mutex;
        std::condition_variable cond;
        std::atomic_bool done = false;
        std::thread worker;

        std::chrono::nanoseconds send_time(const packet & p) const
        {
                return std::chrono::nanoseconds{
                        p.total_size() / bytes_per_second * std::nano::den};
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
        std::size_t bytes_per_second)
    : shared_state_{new shared{next, propagation_delay, bytes_per_second}}
{
        shared_state_->worker = std::thread{[shared = shared_state_]
                                            { worker(std::move(shared)); }};
}


send_result link::send(packet && p)
{
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

                if (p.arrival <= clock::now())
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