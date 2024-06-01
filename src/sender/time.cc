#include "time.hh"
#include <chrono>
#include <cstdint>
#include <iostream>

namespace nimrod
{
send_result timer::send(packet && p)
{
        p.passed_along();

        const auto send_time = std::chrono::steady_clock::now();

        p.set_passed_long_callback(
                [shared = inner_,
                 send_time,
                 span = static_cast<int64_t>(span_)](const packet & p) mutable
                {
                        span -= 1;

                        if (-1 != span)
                                return;

                        auto recv_time = std::chrono::steady_clock::now();
                        auto duration = recv_time - send_time;

                        shared->cb(shared->name, p, duration);
                });

        return next_->send(std::move(p));
}
}