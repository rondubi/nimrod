#include "round_robin.hh"
#include "packet.hh"

namespace nimrod
{
recv_result round_robin::recv(packet * out)
{
        while (true)
        {
                if (receivers_.empty())
                        return recv_result::closed;

                auto i = pos_++ % receivers_.size();
                switch (receivers_[i]->recv(out))
                {
                        case recv_result::closed:
                                receivers_.erase(receivers_.begin() + i);
                                continue;
                        case recv_result::ok: {
                                out->passed_along();
                                return recv_result::ok;
                        }
                }
        }
}
}