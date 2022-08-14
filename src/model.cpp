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

bool Model::validate()
{
	for (const Generator &g : generator_components)
	{
		if (!g.outlet)
			validation_errors.push_back(
			    std::format("Generator {}: outlet can't be empty.", g.name));
		if (g.upper_throughput < g.lower_throughput)
			validation_errors.push_back(
			    std::format("Generator {}: upper throughput is "
			                "lower than lower throughput.",
			                g.name));

		SSFW_ASSERT(g.id, "Generator doesn't have an id.");
		SSFW_ASSERT(g.outlet && g.outlet_id || g.outlet->id != g.outlet_id,
		            "Outlet and outlet ID mismatch.");
	}

	for (const Router &r : router_components)
	{
		if (!r.outlet)
			validation_errors.push_back(
			    std::format("Router {}: outlet 1 can't be empty.", r.name));
		if (!r.outlet_b)
			validation_errors.push_back(
			    std::format("Router {}: outlet 2 can't be empty.", r.name));
		if (r.factor < 0 || r.factor > 1)
			validation_errors.push_back(std::format(
			    "Router {}: factor needs to be be in [0,1].", r.name));

		SSFW_ASSERT(r.id, "Router doesn't have an id.");
		SSFW_ASSERT(r.outlet && r.outlet_id || r.outlet->id != r.outlet_id,
		            "Outlet 1 and outlet ID 1 mismatch.");
		SSFW_ASSERT(r.outlet_b && r.outlet_b_id ||
		                r.outlet_b->id != r.outlet_b_id,
		            "Outlet 2 and outlet ID 2 mismatch.");
	}

	for (const Server &s : server_components)
	{
		if (!s.outlet)
			validation_errors.push_back(
			    std::format("Server {}: outlet can't be empty.", s.name));
		if (s.lower_service_time > s.upper_service_time)
			validation_errors.push_back(
			    std::format("Server {}: lower service time is "
			                "higher than higher service time.",
			                s.name));

		SSFW_ASSERT(s.id, "Server doesn't have an id.");
		SSFW_ASSERT(s.outlet && s.outlet_id || s.outlet->id != s.outlet_id,
		            "Outlet and outletID mismatch.");
	}

	// TODO(Rafael): validate the inlets as well.
	return validation_errors.size() == 0;
}

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
