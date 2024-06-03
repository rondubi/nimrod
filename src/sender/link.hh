#pragma once

#include <chrono>
#include <cstddef>
#include <memory>
#include <numeric>
#include <ratio>
#include <type_traits>

#include "core.hh"

namespace nimrod
{
// template <typename Repr, typename Period = std::ratio<1>>
// struct bytes
// {
//         Repr count;
// };


template <typename Repr, typename Period = std::ratio<1>>
struct bandwidth
{
        Repr count;
};

// template <typename Repr1, typename Repr2, typename Period1, typename Period2>
// bandwidth<
//         std::common_type_t<Repr1, Repr2>,
//         std::ratio<
//                 std::gcd(Period1::num, Period2::num),
//                 std::lcm(Period1::den, Period2::Den)>>
// operator/(
//         const bytes<Repr1, Period1> & a,
//         const std::chrono::duration<Repr2, Period2> & b)
// {
// }


// A link with a set propagation delay and bottleneck bandwidth limit
class link : public sender
{
public:
        ~link();

        explicit link(
                std::shared_ptr<sender> next,
                std::chrono::nanoseconds propagation_delay,
                std::optional<bandwidth<double>> limit);

        send_result send(packet && p) override;

private:
        struct shared;

        static void worker(std::shared_ptr<shared> shared);

        std::shared_ptr<shared> shared_state_;
};
}
