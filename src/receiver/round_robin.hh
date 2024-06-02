#pragma once

#include <memory>
#include <vector>

#include "core.hh"

namespace nimrod
{
class round_robin : public receiver
{
public:
        recv_result recv(packet * out) override;

        void add_receiver(std::shared_ptr<receiver> r);

private:
        std::vector<std::shared_ptr<receiver>> receivers_;
        size_t pos_ = 0;
};
}