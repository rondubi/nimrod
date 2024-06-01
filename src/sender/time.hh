#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>

#include "core.hh"
#include "packet.hh"

namespace nimrod
{


class timer : public sender
{
public:
        using callback = std::function<void(
                std::string_view timer,
                const packet & packet,
                std::chrono::nanoseconds time)>;

        explicit timer(
                std::string name,
                callback cb,
                std::shared_ptr<sender> next,
                std::uint64_t span = 1)
            : inner_{new inner{name, cb}}, next_{next}, span_{span}
        {
        }

        send_result send(packet && p) override;

private:
        struct inner
        {
                std::string name;
                callback cb;
        };

        std::shared_ptr<inner> inner_;
        std::shared_ptr<sender> next_;
        std::uint64_t span_;
};
}