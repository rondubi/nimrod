#include <doctest.h>
#include "core.hh"
#include "packet.hh"

#include <chrono>
#include <thread>
#include "queue.hh"

TEST_CASE("queue single thread")
{
        nimrod::packet p;
        nimrod::queue q{4};

        CHECK(q.empty());
        CHECK(!q.full());

        q.push(nimrod::packet{p});

        CHECK(!q.empty());
        CHECK(!q.full());
        CHECK(q.front() == p);

        q.pop();
        CHECK(q.empty());
        CHECK(!q.full());

        for (int i = 0; i < 4; ++i)
        {
                p.id = i;
                q.push(nimrod::packet{p});
        }

        CHECK(!q.empty());
        CHECK(q.full());

        for (int i = 0; i < 4; ++i)
        {
                p.id = i;
                CHECK(q.front() == p);
                q.pop();
        }

        CHECK(q.empty());
}

TEST_CASE("queue multi thread")
{
        nimrod::queue queue{4};

        std::thread tx{[&queue]
                       {
                               std::this_thread::sleep_for(
                                       std::chrono::milliseconds{20});

                               for (int i = 0; i < 17; ++i)
                               {
                                       nimrod::packet p;
                                       p.id = i;
                                       CHECK(queue.send(std::move(p))
                                             == nimrod::send_result::ok);
                               }
                       }};

        std::thread rx{[&queue]
                       {
                               nimrod::packet p;
                               for (int i = 0; i < 17; ++i)
                               {
                                       CHECK(queue.recv(&p)
                                             == nimrod::recv_result::ok);
                                       CHECK(p.id == i);
                               }
                       }};

        tx.join();
        rx.join();
}