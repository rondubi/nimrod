#include <cstdio>

#include "core.hh"
#include "log.hh"
#include "packet.hh"

namespace nimrod
{
send_result logger::send(packet && p)
{
        p.passed_along();
        printf("received packet!\n");
        return send_result::ok;
}
}