#pragma once

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
                return recv_result::ok;
        }

private:
        packet p_;
};
}