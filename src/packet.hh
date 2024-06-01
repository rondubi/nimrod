#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <variant>

#include "proto/ipv4.hh"

namespace nimrod
{
enum class packet_kind
{
        none,
        ipv4,
};

class packet
{
public:
        packet() = default;

        packet(ipv4_packet packet)
            : kind_{packet_kind::ipv4}, packet_{std::move(packet)}
        {
        }

        std::size_t total_size() const
        {
                struct visitor
                {
                        std::size_t operator()(const ipv4_packet & packet)
                        {
                                return packet.total_size();
                        }

                        std::size_t operator()(const int &) { return 0; }
                };

                return std::visit(visitor{}, packet_);
        }

        packet_kind kind() const { return kind_; }

        ipv4_packet_header & ipv4_header();

        const ipv4_packet_header & ipv4_header() const;

        void passed_along();

        void set_passed_long_callback(std::function<void(const packet &)> cb);

        friend bool operator==(const packet & lhs, const packet & rhs)
        {
                return lhs.kind_ == rhs.kind_ && lhs.packet_ == rhs.packet_;
        }

        friend bool operator!=(const packet & lhs, const packet & rhs)
        {
                return !(lhs == rhs);
        }

        uint64_t id = 0;

private:
        packet_kind kind_ = packet_kind::none;
        std::variant<int, ipv4_packet> packet_{0};
        std::function<void(const packet &)> passed_along_callback_;
};
}