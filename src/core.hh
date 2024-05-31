#pragma once

namespace nimrod
{
class packet;

struct closer
{
        virtual ~closer() = default;
        virtual void close() = 0;
};


enum class send_result
{
        closed,
        ok,
};

struct sender : closer
{
        virtual ~sender() = default;
        // virtual send_result try_send(packet && p) = 0;
        virtual send_result send(packet && p) = 0;
};

enum class recv_result
{
        closed,
        empty,
        ok,
};

struct receiver : closer
{
        virtual ~receiver() = default;
        virtual recv_result recv(packet * out) = 0;
        virtual recv_result recv_blocking(packet * out) = 0;
};
}