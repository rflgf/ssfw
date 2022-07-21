#pragma once

#include <iostream>
#include <string>

namespace ssfw
{

struct event;
using time_unit = size_t;

class component
{
public:
	component(const std::string name, uint8_t id, component *outlet)
	    : name(name), id(id), outlet(outlet)
	{
	}

	component(const char *name, uint8_t id, component *outlet)
	    : name(name), id(id), outlet(outlet)
	{
	}

	component() = default;

	std::string name;
	uint8_t id;
	component *outlet;
	uint8_t outlet_id;

	virtual void evaluate_event(event &e) = 0;
	virtual void update_statistics(event &e) = 0;
	virtual time_unit sample_from_distribution() = 0;
};

} // namespace ssfw