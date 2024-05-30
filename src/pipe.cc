#include "pipe.hh"

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>

namespace nimrod
{
using clock = std::chrono::steady_clock;

class pipe::impl
{
public:
        packet recv_blocking()
        {
                std::unique_lock lock{mutex_};

                while (true)
                {
                        if (queue_.queue.empty())
                        {
                                cond_.wait(lock);
                                continue;
                        }


                        const auto & top = queue_.queue.front();

                        auto recv_time = top.begin_send;
                        recv_time += link_.propagation_delay;
                        recv_time += transmit_time(top.packet);


                        if (clock::now() >= recv_time)
                                return queue_.pop();

                        lock.unlock();
                        std::this_thread::sleep_until(recv_time);
                        lock.lock();
                }
        }

        std::optional<packet> try_recv()
        {
                std::scoped_lock _lock{mutex_};

                if (queue_.queue.empty())
                        return std::nullopt;

                auto & top = queue_.queue.front();

                auto recv_time = top.begin_send;
                recv_time += link_.propagation_delay;
                recv_time += transmit_time(top.packet);

                if (clock::now() < recv_time)
                        return std::nullopt;

                return queue_.pop();
        }

        bool send(packet && p)
        {
                std::scoped_lock _lock{mutex_};

                if (!queue_.can_send(p))
                        return false;

                queue_.send({std::move(p), next_begin_send()});
                cond_.notify_all();
                return true;
        }

private:
        struct link
        {
                std::size_t bytes_per_second;
                std::chrono::nanoseconds propagation_delay;
        };

        struct sent_packet
        {
                packet packet;
                clock::time_point begin_send;
        };


        struct queue
        {
                std::queue<sent_packet> queue;
                std::size_t pending;
                std::size_t capacity;

                std::size_t available_capacity() const
                {
                        assert(pending <= capacity);
                        return capacity - pending;
                }

                [[nodiscard]]
                bool can_send(const packet & p)
                {
                        return available_capacity() >= p.total_size();
                }

                void send(sent_packet && p)
                {
                        assert(can_send(p.packet));
                        pending += p.packet.total_size();
                        queue.push(std::move(p));
                }

                packet pop()
                {
                        assert(!queue.empty());
                        auto p = std::move(queue.front().packet);
                        queue.pop();
                        pending -= p.total_size();
                        return p;
                }
        };


        std::mutex mutex_;
        std::condition_variable cond_;
        queue queue_;
        link link_;

        std::chrono::nanoseconds transmit_time(const packet & p)
        {
                throw "todo";
        }


        clock::time_point next_begin_send()
        {
                auto now = clock::now();
                if (queue_.queue.empty())
                        return now;

                const auto & prev = queue_.queue.back();
                auto prev_end_send
                        = prev.begin_send + transmit_time(prev.packet);

                if (now < prev_end_send)
                        return prev_end_send;

                return now;
        }
};


pipe::~pipe() = default;

packet pipe::recv_blocking()
{
        return pimpl_->recv_blocking();
}

std::optional<packet> pipe::try_recv()
{
        return pimpl_->try_recv();
}

bool pipe::send(packet && p)
{
        return pimpl_->send(std::move(p));
}
}