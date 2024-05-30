#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>

#include "ipfw.hh"

namespace nimrod
{

class pipe
{
public:
        class id
        {
        public:
                friend bool operator==(const id & lhs, const id & rhs)
                {
                        return lhs.inner == rhs.inner;
                }

                friend bool operator!=(const id & lhs, const id & rhs)
                {
                        return !(lhs == rhs);
                }


        private:
                std::uint32_t inner;
        };

        ~pipe();

        packet recv_blocking();

        std::optional<packet> try_recv();

        bool send(packet && p);


private:
        class impl;

        std::unique_ptr<impl> pimpl_;

public:
        const id id;
};
}