#pragma once

#include "component.h"
#include "core.h"

namespace ssfw
{

class sink : public component
{
public:
	struct statistics
	{
		entity entity_count;
		time_unit avg_entity_lifetime;
	};

	statistics statistics;

	sink(const std::string name, uint8_t id) : component(name, id, nullptr) {}

	sink(const char *name, uint8_t id) : component(name, id, nullptr) {}

	virtual void evaluate_event(event &e) override;
	virtual void update_statistics(event &e) override;
	virtual time_unit sample_from_distribution() override;
};

} // namespace ssfw
