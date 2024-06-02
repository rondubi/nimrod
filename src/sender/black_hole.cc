#include "black_hole.hh"
#include "core.hh"
#include "packet.hh"

namespace nimrod
{
send_result black_hole::send(packet && p)
{
        p.passed_along();
        return send_result::ok;
}
}