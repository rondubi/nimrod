#pragma once

#include <chrono>
#include <memory>
#include <thread>

#include "core.hh"

namespace nimrod
{
struct fixed_delay : public sender
{
        explicit fixed_delay(
                std::chrono::nanoseconds d, std::shared_ptr<sender> s)
            : delay{d}, next{std::move(s)}
        {
        }

        std::chrono::nanoseconds delay;
        std::shared_ptr<sender> next;

        send_result send(packet && p) override
        {
                std::this_thread::sleep_for(delay);
                return next->send(std::move(p));
        }
};
}