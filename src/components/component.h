#pragma once

#include <iostream>
#include <string>

#include "core.h"
#include "event.h"

namespace ssfw
{

class Component
{
public:
	Component(const std::string name, uint8_t id, Component *outlet)
	    : name(name), id(id), outlet(outlet)
	{
	}

	Component(const char *name, uint8_t id, Component *outlet)
	    : name(name), id(id), outlet(outlet)
	{
	}

	Component() = default;

	std::string name;
	uint8_t id;
	Component *outlet;
	uint8_t outlet_id;

	virtual void evaluate_event(Event &e) = 0;
	virtual void update_statistics(Event &e) = 0;
	virtual time_unit sample_from_distribution() = 0;
};

} // namespace ssfw