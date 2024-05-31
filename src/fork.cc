#include "fork.hh"
#include "core.hh"
#include "packet.hh"

namespace nimrod
{
send_result fork::send(packet && p)
{
        for (size_t i = 0; i < senders_.size();)
        {
                auto & s = senders_[i];


                send_result res;

                if (i == senders_.size() - 1)
                        res = s->send(std::move(p));
                else
                        res = s->send(packet{p});

                switch (res)
                {
                        case send_result::closed:
                                senders_.erase(senders_.begin() + i);
                                break;
                        case send_result::ok:
                                i++;
                                break;
                }
        }

        if (senders_.empty())
                return send_result::closed;
        return send_result::ok;
}
}