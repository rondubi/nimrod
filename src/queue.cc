#include "queue.hh"
#include "core.hh"

namespace nimrod
{
// send_result queue::try_send(packet && p)
// {
//         if (closed_)
//                 return send_result::closed;
//         if (full())
//                 return send_result::full;

//         push(std::move(p));
//         return send_result::ok;
// }

send_result queue::send(packet && p)
{
        if (closed_)
                return send_result::closed;

        const auto head = head_.load();
        auto tail = tail_.load();

        while (true)
        {
                if ((head - tail) >= size_)
                {
                        tail_.wait(tail);
                        if (closed_)
                                return send_result::closed;

                        tail = tail_.load();
                        continue;
                }

                assert(!full());
                push(std::move(p));
                return send_result::ok;
        }
}

recv_result queue::recv(packet * out)
{
        if (closed_)
                return recv_result::closed;
        if (empty())
                return recv_result::empty;

        *out = std::move(front());
        pop();
        return recv_result::ok;
}

recv_result queue::recv_blocking(packet * out)
{
        if (closed_)
                return recv_result::closed;

        auto head = head_.load();
        const auto tail = tail_.load();

        while (true)
        {
                if (head == tail)
                {
                        head_.wait(head);
                        head = head_.load();
                        if (closed_)
                                return recv_result::closed;
                        continue;
                }


                assert(!empty());
                *out = std::move(front());
                pop();
                return recv_result::ok;
        }
}

void queue::close()
{
        closed_.store(true);
        head_.notify_one();
        tail_.notify_one();
}

void queue::push(packet && p)
{
        assert(!full());
        auto head = head_.load(std::memory_order_relaxed);
        buf_[head % size_] = std::move(p);
        head_.store(head + 1, std::memory_order_release);
        head_.notify_one();
}

void queue::pop()
{
        assert(!empty());
        auto tail = tail_.load(std::memory_order_relaxed);
        tail_.store(tail + 1, std::memory_order_release);
        tail_.notify_one();
}

}