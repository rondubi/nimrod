#pragma once

#include "ipfw.hh"

#include <memory>

struct Expr
{
        virtual bool match(packet) const = 0;
};

struct LengthMatch : Expr
{
        enum field
        {
                ipv4_from,
                ipv4_to
        };

        // NOTE: this only works insofar as the types are ints
        bool match(packet p) const
        {
                const int c = field == field::ipv4_from ? p.ipv4_from : p.ipv4_to;
                
                for (int i = 0; i < match_length; ++i)
                {
                        // 0 24
                        // 1 16
                        // 2 8
                        // 3 0
                        if (i & (0xff << (24 - 8 * i)) == val
                }

                return true;
        }

        ExactMatch(field f, int v, int match_length_)
        : field(f), val(v), match_length(match_length_) {}

        field field;
        int val;
        int match_length;
};

struct ExactMatch : Expr
{
        enum field
        {
                ipv4_from,
                ipv4_to
        };

        bool match(packet p) const
        {
                return (field == field::ipv4_from ? p.ipv4_from : p.ipv4_to) == val;
        }

        ExactMatch(field f, int v) : field(f), val(v) {}

        field field;
        int val;
};

