#pragma once

#include <chrono>
#include <cstddef>
#include <memory>
#include <vector>

#include "core.hh"

namespace nimrod
{
std::vector<std::shared_ptr<sender>> case_a(std::size_t flows);

std::vector<std::shared_ptr<sender>>
case_b(std::size_t flows, std::size_t rules);

std::vector<std::shared_ptr<sender>>
case_c(std::size_t flows,
       std::chrono::nanoseconds delay,
       std::size_t bandwidth);
}
