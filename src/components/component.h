#pragma once

#include "core.h"

namespace ssfw
{

class component
{
public:
	component(const std::string &name, uint8_t id, component *outlet)
	    : name(name), id(id), outlet(outlet)
	{
	}

	component(const char *name, uint8_t id, component *outlet)
	    : name(name), id(id), outlet(outlet)
	{
	}

	const std::string &name;
	uint8_t id;
	component *outlet;

	virtual void update_statistics(event &e) = 0;
};

} // namespace ssfw
