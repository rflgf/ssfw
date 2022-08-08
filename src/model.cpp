#include <fstream>
#include <iostream>

#include "model.h"

#include "components/generator.h"
#include "components/router.h"
#include "components/server.h"
#include "components/sink.h"

namespace ssfw
{

Model::Model(const char *name, time_unit sim_time_limit)
    : name(name), sim_time_limit(sim_time_limit)
{
	// hardcoding componentes por hora.
	Sink *s1 = new Sink("Saída A", 3);
	Sink *s2 = new Sink("Saída B", 4);
	Router *r = new Router("Roteador A", 2, s1, s2, 0.3f);
	Server *s = new Server("Centro de Serviço A", 1, r, 2, 5, 2);
	Generator *g = new Generator("Entrada A", 0, s, 1, 3);

	components.push_back(g);
	generators.push_back(g);
	components.push_back(s);
	components.push_back(r);
	components.push_back(s1);
	components.push_back(s2);
}

Model::~Model()
{
	for (auto c : components)
		delete c;
}

Model Model::load_from(std::string_view file_path)
{
	return Model("Placeholder", 10000);
}

void Model::save(const char *file_path)
{
	std::ofstream file(file_path);
	file << json(*this).dump();
	file.close();
}

void Model::validate() {}

void Model::update_statistics(Event &e) {}

using json = nlohmann::json;

void to_json(json &j, const Model &m)
{
	j = json {{"name", m.name},
	          {"sim_time_limit", m.sim_time_limit},
	          {"generators", m.generator_components},
	          {"routers", m.router_components},
	          {"servers", m.server_components},
	          {"sinks", m.sink_components}};
}

void from_json(const json &j, Model &m)
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

	std::unordered_map<uint8_t, Component *> components_by_id;

	for (auto &c : m.generator_components)
		components_by_id.insert({c.id, static_cast<Component *>(&c)});
	for (auto &c : m.router_components)
		components_by_id.insert({c.id, static_cast<Component *>(&c)});
	for (auto &c : m.server_components)
		components_by_id.insert({c.id, static_cast<Component *>(&c)});
	for (auto &c : m.sink_components)
		components_by_id.insert({c.id, static_cast<Component *>(&c)});

	for (auto iter = components_by_id.begin(); iter != components_by_id.end();
	     ++iter)
	{
		Component *c = iter->second;
		SSFW_ASSERT(components_by_id.find(c->outlet_id) !=
		                components_by_id.end(),
		            "Malformed input JSON: component missing.");
		c->outlet = components_by_id.at(c->outlet_id);
	}
}

} // namespace ssfw
