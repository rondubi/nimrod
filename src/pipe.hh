#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>

namespace nimrod
{
class queue
{
public:
        bool empty() const { return pending() == 0; }
        bool full() const { return pending() >= capacity_; }

        std::size_t pending() const { return pending_; }
        std::size_t capacity() const { return capacity_; }

private:
        std::size_t pending_;
        std::size_t capacity_;
};

class link
{
public:
        std::size_t bytes_per_second() const { return bytes_per_second_; }

        std::chrono::nanoseconds propagation_delay() const
        {
                return propagation_delay_;
        }

private:
        std::size_t bytes_per_second_;
        std::chrono::nanoseconds propagation_delay_;
};

class pipe_id
{
public:
        friend bool operator==(const pipe_id & lhs, const pipe_id & rhs)
        {
                return lhs.inner == rhs.inner;
        }

        friend bool operator!=(const pipe_id & lhs, const pipe_id & rhs)
        {
                return !(lhs == rhs);
        }


private:
        std::uint32_t inner;
};

struct pipe
{
        queue queue;
        link link;
        pipe_id id;
};
}