#pragma once

#include <atomic>
#include <cassert>
#include <memory>

#include "core.hh"
#include "packet.hh"

namespace nimrod
{
// spsc queue
class queue : public sender, public receiver
{
public:
        explicit queue(std::size_t capacity)
            : buf_{std::make_unique<packet[]>(capacity)}, size_{capacity}
        {
        }

        // send_result try_send(packet && p) override;
        send_result send(packet && p) override;
        recv_result recv(packet * out) override;
        recv_result recv_blocking(packet * out) override;
        void close() override;

        bool full() const { return (head_ - tail_) >= size_; }
        bool empty() const { return head_ == tail_; }

        void push(packet && p);

        void pop();

        packet & front()
        {
                assert(!empty());
                return buf_[tail_ % capacity()];
        }

        std::size_t capacity() const { return size_; }

private:
        std::unique_ptr<packet[]> buf_;
        std::size_t size_;
        std::atomic_size_t head_ = 0;
        std::atomic_size_t tail_ = 0;
        std::atomic_bool closed_ = false;
};
}