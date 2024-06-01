#include <thread>

#include "core.hh"
#include "fuse.hh"
#include "packet.hh"

namespace nimrod
{
namespace
{
void fuse_impl(receiver & r, sender & s)
{
        packet packet;

        while (true)
        {
                switch (r.recv(&packet))
                {
                        case recv_result::closed:
                                return;
                        case recv_result::ok:
                                break;
                }

                switch (s.send(std::move(packet)))
                {
                        case send_result::closed:
                                return;
                        case send_result::ok:
                                break;
                }
        }
}
}

void fuse(std::shared_ptr<receiver> recv, std::shared_ptr<sender> send)
{
        std::thread{[recv = std::move(recv), send = std::move(send)] {
                fuse_impl(*recv, *send);
        }}.detach();
}
}