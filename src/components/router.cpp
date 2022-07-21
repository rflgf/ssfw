#include <cstdlib>

#include "router.h"

namespace ssfw
{

namespace simulation
{
extern std::multimap<time_unit, std::unique_ptr<event>> events;
}

void router::evaluate_event(event &e)
{
	constexpr int division_magic_number_i = 10000;
	constexpr float division_magic_number_f = 1.0f / division_magic_number_i;
	float percentage = std::rand() * division_magic_number_f;
	component *out = percentage >= factor ? get_outlet_a() : get_outlet_b();

	simulation::events.emplace(
	    std::piecewise_construct, std::forward_as_tuple(e.start),
	    std::forward_as_tuple(std::make_unique<event>(
	        e.ent, out, e.start, sample_from_distribution())));
}

void router::update_statistics(event &e) {}

time_unit router::sample_from_distribution() { return 0; }

using json = nlohmann::json;

void to_json(json &j, const ssfw::router &r)
{
	j = json {{"id", r.id},
	          {"name", r.name},
	          {"type", "router"},
	          {"factor", r.factor},
	          {"outlet_a", r.outlet->id},
	          {"outlet_b", r.outlet_b->id}};
}

void from_json(const json &j, ssfw::router &r)
{
	SSFW_ASSERT(std::string(j.at("type")).compare("router"),
	            "Attempt to read non-router JSON as a router object.");

	j.at("id").get_to(r.id);
	j.at("name").get_to(r.name);
	j.at("factor").get_to(r.factor);
	j.at("outlet_a").get_to(r.outlet_id);
	j.at("outlet_b").get_to(r.outlet_b_id);
}

} // namespace ssfw