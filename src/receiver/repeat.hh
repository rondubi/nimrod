#pragma once

#include <vector>

#include "core.hh"
#include "packet.hh"

namespace nimrod
{
class repeat : public receiver
{
public:
        explicit repeat(packet p) : p_{std::move(p)} { }

        recv_result recv(packet * out) override
        {
                *out = p_;
                out->passed_along();
                return recv_result::ok;
        }

private:
        packet p_;
};
}
