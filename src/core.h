#pragma once

#include <iostream>
#include <list>
#include <map>
#include <string>

#include "components/component.h"

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

	friend std::ostream &operator<<(std::ostream &stream, const event &e)
	{
		if (!e.comp)
			return stream;

		stream << "event: entity " << e.ent << " at component " << e.comp->name
		       << ", with start " << e.start << " and duration " << e.duration
		       << ", with end " << e.start + e.duration;
		return stream;
	}
};

} // namespace ssfw
