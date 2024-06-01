#pragma once

#include <chrono>
#include <memory>

#include "core.hh"

namespace nimrod
{
// A link with a set propagation delay and bottleneck bandwidth limit
class link : public sender
{
public:
        ~link();

        explicit link(
                std::shared_ptr<sender> next,
                std::chrono::nanoseconds propagation_delay,
                std::size_t bytes_per_second);

        send_result send(packet && p) override;

private:
        struct shared;

        static void worker(std::shared_ptr<shared> shared);

        std::shared_ptr<shared> shared_state_;
};
}