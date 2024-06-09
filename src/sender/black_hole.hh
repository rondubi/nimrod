#pragma once

#include "core.hh"

namespace nimrod
{
struct black_hole : sender
{
        black_hole() = default;

        send_result send(packet && p) final;
};
}