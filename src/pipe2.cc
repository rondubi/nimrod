#include "pipe2.hh"

#include <chrono>
#include <memory>
#include <thread>
#include <variant>

namespace nimrod
{
namespace
{
void worker(
        receiver & src,
        sender & dst,
        std::size_t bytes_per_second,
        std::chrono::nanoseconds propagation_delay)
{
        // for (;;)
        // {
        //         auto recv_result = src.recv_blocking();

        //         if (std::holds_alternative<closed>(recv_result))
        //                 return;

        //         assert(std::holds_alternative<packet>(recv_result));
        //         auto & packet = std::get<0>(recv_result);

        //         dst.send(std::move(packet));
        // }
}
}

pipe2::~pipe2()
{
        worker_.detach();
}

pipe2::pipe2(
        std::shared_ptr<receiver> src,
        std::shared_ptr<sender> dst,
        std::size_t bytes_per_second,
        std::chrono::nanoseconds propagation_delay)
    : inner_{std::make_shared<inner>(
              inner{std::move(src),
                    std::move(dst),
                    bytes_per_second,
                    propagation_delay})}
{
        worker_ = std::thread{[inner = inner_]
                              {
                                      worker(*inner->src,
                                             *inner->dst,
                                             inner->bytes_per_second,
                                             inner->propagation_delay);
                              }};
}
}