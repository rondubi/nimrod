#pragma once

#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <thread>

#include "core.hh"
#include "packet.hh"
#include "queue.hh"

namespace nimrod
{
// The input source to a pipe. This can be a scheduler or queue
class pipe_source
{
public:
        virtual ~pipe_source() = default;

        virtual std::optional<packet> try_next() = 0;
        virtual packet next_blocking() = 0;
};


class fifo_queue : public pipe_source
{
public:
        std::optional<packet> try_next() final
        {
                if (queue_.empty())
                        return std::nullopt;

                auto p = std::move(queue_.front());
                queue_.pop();
                return p;
        }

        packet next_blocking() final;

        bool full() const { return queue_.full(); }

        void push(packet && p) { queue_.push(std::move(p)); }

private:
        queue queue_;
};

class pipe2
{
public:
        ~pipe2();

        explicit pipe2(
                std::shared_ptr<receiver> src,
                std::shared_ptr<sender> dst,
                std::size_t bytes_per_second,
                std::chrono::nanoseconds propagation_delay);

private:
        struct inner
        {
                std::shared_ptr<receiver> src;
                std::shared_ptr<sender> dst;
                std::size_t bytes_per_second;
                std::chrono::nanoseconds propagation_delay;
        };

        std::shared_ptr<inner> inner_;
        std::thread worker_;
};


// class round_robin_scheduler : public receiver
// {
// private:
//         std::optional<packet> recv() final
//         {
//                 size_t start = pos_;
//                 size_t end = start + queues_.size();

//                 for (size_t i = start; i < end; ++i)
//                 {
//                         auto & queue = queues_[i % queues_.size()];
//                         if (queue->full())
//                         {
//                         }
//                 }

//                 for (auto & queue : queues_)
//                 {
//                         if (queue->empty())
//                                 continue;
//                         auto p = std::move(queue->front());
//                         queue->pop();
//                         return p;
//                 }
//                 return std::nullopt;
//         }

//         packet recv_blocking() final;

// public:
//         std::vector<std::shared_ptr<queue>> queues_;
//         std::size_t pos_ = 0;
// };
}