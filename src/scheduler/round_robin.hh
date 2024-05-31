#pragma once

#include <memory>
#include <vector>

#include "core.hh"

namespace nimrod
{
class round_robin_scheduler : public receiver
{
public:
        round_robin_scheduler() = default;

        void add_receiver(std::shared_ptr<receiver> r)
        {
                receivers_.push_back(std::move(r));
        }

        void close() override
        {
                for (auto & r : receivers_)
                        r->close();
                receivers_.clear();
        }

        recv_result recv(packet * out) override
        {
                if (receivers_.empty())
                        return recv_result::closed;


                auto start = pos_;
                auto end = pos_ + receivers_.size();

                for (; pos_ < end;)
                {
                        if (receivers_.empty())
                                return recv_result::closed;

                        const auto pos = receivers_.begin()
                                + (pos_ % receivers_.size());

                        auto & r = *pos;
                        pos_++;

                        auto res = r->recv(out);

                        switch (res)
                        {
                                case recv_result::ok:
                                        return recv_result::ok;
                                case recv_result::empty:
                                        continue;
                                case recv_result::closed: {
                                        receivers_.erase(pos);
                                        continue;
                                }
                        }
                }

                return recv_result::empty;
        }

        recv_result recv_blocking(packet * out) override;

private:
        std::vector<std::shared_ptr<receiver>> receivers_;
        std::size_t pos_ = 0;
};
}