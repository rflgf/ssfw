#pragma once

#include <iostream>
#include <list>
#include <map>
#include <string>

namespace ssfw
{

class component;

using entity = size_t;
using time_unit = size_t;
using event = std::tuple<entity, component &, size_t,
                         size_t>; // entity, component, start, duration

} // namespace ssfw
