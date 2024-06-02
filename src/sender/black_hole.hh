#pragma once

#include "core.hh"

namespace nimrod
{
struct black_hole : sender
{
        send_result send(packet && p) override;
};
}