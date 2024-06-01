#pragma once

#include <cstddef>
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

                        std::size_t operator()(const std::nullopt_t &)
                        {
                                return 0;
                        }
                };

                return std::visit(visitor{}, packet_);
        }

        packet_kind kind() const { return kind_; }

        ipv4_packet_header & ipv4_header();

        const ipv4_packet_header & ipv4_header() const;

        void passed_along();

        void set_passed_long_callback(std::function<void(const packet &)> cb);

private:
        packet_kind kind_ = packet_kind::none;
        std::variant<std::nullopt_t, ipv4_packet> packet_{std::nullopt};
        std::function<void(const packet &)> passed_along_callback_;
};
}