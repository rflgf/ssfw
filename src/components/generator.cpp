#include "generator.h"

namespace ssfw
{

void generator::evaluate_event(event &e) { e.comp->evaluate_event(e); }

void generator::update_statistics(event &e) {}

time_unit generator::sample_from_distribution() { return 0; }

using json = nlohmann::json;

void to_json(json &j, const generator &g)
{
	j = json {{"id", g.id},
	          {"name", g.name},
	          {"type", "generator"},
	          {"lower_throughput", g.lower_throughput},
	          {"upper_throughput", g.upper_throughput},
	          {"outlet", g.outlet->id}};
}

void from_json(const json &j, generator &g)
{
	SSFW_ASSERT(std::string(j.at("type")).compare("generator"),
	            "Attempt to read non-generator JSON into a generator object.");

	j.at("id").get_to(g.id);
	j.at("name").get_to(g.name);
	j.at("lower_throughput").get_to(g.lower_throughput);
	j.at("upper_throughput").get_to(g.upper_throughput);
	j.at("outlet").get_to(g.outlet_id);
}

} // namespace ssfw