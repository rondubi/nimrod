#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace nimrod
{
struct ipv4_addr
{
        std::uint32_t bits;

        friend bool operator==(const ipv4_addr & lhs, const ipv4_addr & rhs)
        {
                return lhs.bits == rhs.bits;
        }

        friend bool operator!=(const ipv4_addr & lhs, const ipv4_addr & rhs)
        {
                return !(lhs == rhs);
        }
};

enum class ipv4_version : uint8_t
{
};

struct ipv4_packet_header
{
        ipv4_version version;
        uint8_t service;
        uint16_t total_length = sizeof(ipv4_packet_header);
        uint16_t id;
        uint16_t flags_and_offset;
        uint8_t ttl;
        uint8_t proto;
        uint16_t checksum;
        ipv4_addr src;
        ipv4_addr dst;
        uint32_t options;

        friend bool operator==(
                const ipv4_packet_header & lhs, const ipv4_packet_header & rhs)
        {
                return lhs.version == rhs.version && lhs.service == rhs.service
                        && lhs.total_length == rhs.total_length
                        && lhs.id == rhs.id
                        && lhs.flags_and_offset == rhs.flags_and_offset
                        && lhs.ttl == rhs.ttl && lhs.proto == rhs.proto
                        && lhs.checksum == rhs.checksum && lhs.src == rhs.src
                        && lhs.dst == rhs.dst && lhs.options == rhs.options;
        }
        friend bool operator!=(
                const ipv4_packet_header & lhs, const ipv4_packet_header & rhs)
        {
                return !(lhs == rhs);
        }
};

struct ipv4_packet
{
        ipv4_packet_header header;
        std::vector<std::byte> payload;

        std::size_t total_size() const { return header.total_length; }

        friend bool operator==(const ipv4_packet & lhs, const ipv4_packet & rhs)
        {
                return lhs.header == rhs.header && lhs.payload == rhs.payload;
        }
        friend bool operator!=(const ipv4_packet & lhs, const ipv4_packet & rhs)
        {
                return !(lhs == rhs);
        }
};
}