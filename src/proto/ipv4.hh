#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace nimrod
{
struct ipv4_addr
{
        std::uint32_t bits;
};

enum class ipv4_version : uint8_t
{
};

struct ipv4_packet_header
{
        ipv4_version version;
        uint8_t service;
        uint16_t total_length;
        uint16_t id;
        uint16_t flags_and_offset;
        uint8_t ttl;
        uint8_t proto;
        uint16_t checksum;
        ipv4_addr src;
        ipv4_addr dst;
        uint32_t options;
};

struct ipv4_packet
{
        ipv4_packet_header header;
        std::vector<std::byte> payload;

        std::size_t total_size() const { return header.total_length; }
};
}