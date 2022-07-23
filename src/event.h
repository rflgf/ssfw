#pragma once

#include "core.h"

namespace ssfw
{

class component;

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

	friend std::ostream &operator<<(std::ostream &stream, const event &e);
};

} // namespace ssfw