#pragma once

#include <memory>
#include <vector>

#include "core.hh"

namespace nimrod
{
class fork : public sender
{
public:
        fork() = default;

        send_result send(packet && p) override;

private:
        std::vector<std::shared_ptr<sender>> senders_;
};
}