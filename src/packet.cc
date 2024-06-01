#include "packet.hh"
#include "proto/ipv4.hh"

namespace nimrod
{
ipv4_packet_header & packet::ipv4_header()
{
        return std::get<ipv4_packet>(packet_).header;
}

const ipv4_packet_header & packet::ipv4_header() const
{
        return std::get<ipv4_packet>(packet_).header;
}

void packet::passed_along()
{
        if (passed_along_callback_)
                passed_along_callback_(*this);
}

void packet::set_passed_long_callback(std::function<void(const packet &)> cb)
{
        passed_along_callback_ = std::move(cb);
}
}