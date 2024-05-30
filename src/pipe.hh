#pragma once

#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>

#include "ipfw.hh"

namespace nimrod
{
struct pipe_options;

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

class pipe
{
public:
        ~pipe();

        explicit pipe(const pipe_options & options);

        packet recv_blocking();

        std::optional<packet> try_recv();

        bool send(packet && p);


private:
        class impl;

        std::unique_ptr<impl> pimpl_;

public:
        const pipe_id id;
};

struct link_options
{
        std::size_t bytes_per_second;
        std::chrono::nanoseconds propagation_delay;
};

struct queue_options
{
        std::size_t size;
};

struct pipe_options
{
        pipe_id id;
        queue_options queue;
        link_options link;
};
}