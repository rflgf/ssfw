#pragma once

#include "components/component.h"
#include "components/generator.h"
#include "components/router.h"
#include "components/server.h"
#include "components/sink.h"
#include "core.h"

namespace ssfw
{

class Model
{
public:
	struct Statistics
	{
		time_unit avg_entity_lifetime;
	};

	time_unit elapsed_time = 0;
	std::string name;
	time_unit sim_time_limit;
	std::vector<Component *> components;
	std::vector<Generator *> generators;

	std::vector<Generator> generator_components;
	std::vector<Router> router_components;
	std::vector<Server> server_components;
	std::vector<Sink> sink_components;

	Model(const std::string name, time_unit sim_time_limit)
	    : name(name), sim_time_limit(sim_time_limit)
	{
	}

	Model(const char *name, time_unit sim_time_limit);

	~Model();

	Model load_from(std::string_view file_path);
	void validate();
	void save(const char *file_path);
	void update_statistics(Event &e);

	inline std::vector<Generator *> get_generators() const
	{
		return generators;
	};

	friend class Application;

	using json = nlohmann::json;
	friend void to_json(json &j, const Model &m);
	friend void from_json(const json &j, Model &m);

private:
	Model() {}
};

} // namespace ssfw
