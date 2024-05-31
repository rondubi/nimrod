#pragma once

#include "core.hh"

namespace nimrod
{
class logger : public sender
{
public:
        logger() = default;

        send_result send(packet && p) override;
};
}