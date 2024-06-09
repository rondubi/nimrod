#include "black_hole.hh"
#include "core.hh"
#include "packet.hh"

namespace nimrod
{
send_result black_hole::send(packet && p)
{
        p.passed_along();
        auto owned = std::move(p);
        auto ptr = (volatile uint8_t *)&owned;


        std::uint8_t byte = ptr[5];
        ptr[5] = 20;

        return send_result::ok;
}
}