#pragma once

#include "core.h"

namespace ssfw
{

class Component;

struct Event
{
	entity ent;
	Component *component;
	time_unit start;
	time_unit duration;

	Event(entity ent, Component *component, time_unit start, time_unit duration)
	    : ent(ent), component(component), start(start), duration(duration)
	{
	}

	friend std::ostream &operator<<(std::ostream &stream, const Event &e);
};

} // namespace ssfw