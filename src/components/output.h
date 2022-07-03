#pragma once

#include "component.h"
#include "core.h"

namespace ssfw
{

class output : public component
{
public:
	struct statistics
	{
		entity entity_count;
		time_unit avg_entity_lifetime;
	};

	statistics statistics;

	output(std::string name, uint32_t id) : component(name, id, nullptr) {}

	output(const char *name, uint32_t id) : component(name, id, nullptr) {}

	virtual void update_statistics(event &e) override;
};

} // namespace ssfw
