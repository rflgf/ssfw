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

struct event
{
	entity ent;
	component *comp;
	time_unit start;
	time_unit duration;

	event(entity ent, component *comp, time_unit start, time_unit duration)
	    : ent(ent), comp(comp), start(start), duration(duration)
	{
	}
};

} // namespace ssfw
