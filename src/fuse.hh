#pragma once

#include <memory>

#include "core.hh"

namespace nimrod
{
// Fuse a receiver and a sender together
void fuse(std::shared_ptr<receiver> recv, std::shared_ptr<sender> send);
}