#pragma once

#include "components/component.h"
#include "components/generator.h"
#include "core.h"

namespace ssfw
{

class model
{
public:
	struct statistics
	{
		time_unit avg_entity_lifetime;
	};

	time_unit elapsed_time = 0;
	std::string name;
	time_unit sim_time_limit;
	std::list<component *> components;
	std::list<generator *> generators;

	model(const std::string name, time_unit sim_time_limit)
	    : name(name), sim_time_limit(sim_time_limit)
	{
	}

	model(const char *name, time_unit sim_time_limit);

	~model();

	model load_from(std::string_view file_path);
	void validate();
	void update_statistics(event &e);
	inline std::list<generator *> get_generators() const { return generators; };

private:
	model() {}
};

} // namespace ssfw
