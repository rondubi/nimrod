#pragma once

#include <cstdint>
#include <cstdio>
#include <memory>
#include "proto/ipv4.hh"

namespace nimrod
{
struct Expr
{
        virtual bool match(ipv4_addr val) const = 0;
        virtual ~Expr() = default;
};

struct And : Expr
{
        std::unique_ptr<Expr> a;
        std::unique_ptr<Expr> b;

        And(Expr * a_, Expr * b_) : a(a_), b(b_) { }

        bool match(ipv4_addr v) const { return a->match(v) && b->match(v); }
};

struct Or : Expr
{
        std::unique_ptr<Expr> a;
        std::unique_ptr<Expr> b;

        Or(Expr * a_, Expr * b_) : a(a_), b(b_) { }

        bool match(ipv4_addr v) const { return a->match(v) || b->match(v); }
};

struct Not : Expr
{
        std::unique_ptr<Expr> a;

        Not(Expr * a_) : a(a_) { }

        bool match(ipv4_addr v) const { return !a->match(v); }
};

struct LengthMatch : Expr
{
        // NOTE: this only works insofar as the types are 4 bytes wide
        bool match(ipv4_addr v) const
        {
                for (int i = 0; i < match_length; ++i)
                {
                        // 0 24
                        // 1 16
                        // 2 8
                        // 3 0
                        if ((v.bits
                             & static_cast<uint32_t>(0xff << (24 - 8 * i)))
                            != (val.bits
                                & static_cast<uint32_t>(0xff << (24 - 8 * i))))
                                return false;
                }

                return true;
        }

        LengthMatch(ipv4_addr v, int match_length_)
            : val(v), match_length(match_length_)
        {
        }

        ipv4_addr val;
        int match_length;
};

struct ExactMatch : Expr
{
        bool match(ipv4_addr v) const { return v == val; }

        ExactMatch(ipv4_addr v) : val(v) { }

        ipv4_addr val;
};
}