#pragma once


#include <cstddef>
#include <memory>

#include "core.hh"
#include "packet.hh"

namespace nimrod
{
class limit : public receiver
{
public:
        explicit limit(std::size_t limit, std::shared_ptr<receiver> recv)
            : remaining_{limit}, receiver_{std::move(recv)}
        {
        }

        recv_result recv(packet * out) override
        {
                if (remaining_ == 0)
                {
                        receiver_ = {};
                        return recv_result::closed;
                }

                remaining_--;

                auto res = receiver_->recv(out);

                if (res == recv_result::closed)
                {
                        remaining_ = 0;
                        receiver_ = {};
                }
                else
                        out->passed_along();

                return res;
        }

private:
        std::size_t remaining_;
        std::shared_ptr<receiver> receiver_;
};
}