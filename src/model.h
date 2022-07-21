#pragma once

#include "components/component.h"
#include "components/generator.h"
#include "components/router.h"
#include "components/server.h"
#include "components/sink.h"
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
	std::vector<component *> components;
	std::vector<generator *> generators;

	std::vector<generator> generator_components;
	std::vector<router> router_components;
	std::vector<server> server_components;
	std::vector<sink> sink_components;

	model(const std::string name, time_unit sim_time_limit)
	    : name(name), sim_time_limit(sim_time_limit)
	{
	}

	model(const char *name, time_unit sim_time_limit);

	~model();

	model load_from(std::string_view file_path);
	void validate();
	void save(const char *file_path);
	void update_statistics(event &e);

	inline std::vector<generator *> get_generators() const
	{
		return generators;
	};

	using json = nlohmann::json;
	friend void to_json(json &j, const model &m);
	friend void from_json(const json &j, model &m);

private:
	model() {}
};

} // namespace ssfw
