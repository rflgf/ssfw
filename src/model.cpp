#include <fstream>
#include <iostream>

#include "model.h"

#include "components/generator.h"
#include "components/router.h"
#include "components/server.h"
#include "components/sink.h"

namespace ssfw
{

model::model(const char *name, time_unit sim_time_limit)
    : name(name), sim_time_limit(sim_time_limit)
{
	// hardcoding componentes por hora.
	sink *s1 = new sink("Saída A", 3);
	sink *s2 = new sink("Saída B", 4);
	router *r = new router("Roteador A", 2, s1, s2, 0.3f);
	server *s = new server("Centro de Serviço A", 1, r, 2, 5, 2);
	generator *g = new generator("Entrada A", 0, s, 1, 3);

	components.push_back(g);
	generators.push_back(g);
	components.push_back(s);
	components.push_back(r);
	components.push_back(s1);
	components.push_back(s2);
}

model::~model()
{
	for (auto c : components)
		delete c;
}

model model::load_from(std::string_view file_path)
{
	return model("Placeholder", 10000);
}

void model::save(const char *file_path)
{
	std::ofstream file(file_path);
	file << json(*this).dump();
	file.close();
}

void model::validate() {}

void model::update_statistics(event &e) {}

using json = nlohmann::json;

void to_json(json &j, const ssfw::model &m)
{
	j = json {{"name", m.name},
	          {"sim_time_limit", m.sim_time_limit},
	          {"generators", m.generator_components},
	          {"routers", m.router_components},
	          {"servers", m.server_components},
	          {"sinks", m.sink_components}};
}

void from_json(const json &j, ssfw::model &m)
{
	j.at("name").get_to(m.name);
	j.at("sim_time_limit").get_to(m.sim_time_limit);
	j.at("generators").get_to(m.generator_components);
	j.at("routers").get_to(m.router_components);
	j.at("servers").get_to(m.server_components);
	j.at("sinks").get_to(m.sink_components);

	// upon deserialization, each component object only has its `outlet_id`
	// attribute properly set, so we need to look up the location in memory
	// given to the actual outlet component and make it so `component.outlet`
	// actually points to it. for now, since we're providing a model creation
	// tool, and given the scope of this project, it's fair to assume that the
	// input JSON file is always valid and no further validation will be made.
	// TODO(Rafael): do the thing described above.
}

} // namespace ssfw
