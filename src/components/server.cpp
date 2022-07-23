#include "server.h"

namespace ssfw
{

namespace simulation
{
extern time_unit elapsed_time;
extern std::multimap<time_unit, std::unique_ptr<event>> events;
} // namespace simulation

void server::update_statistics(event &e) {}

void server::evaluate_event(event &e)
{
	time_unit delay = sample_from_distribution();
	if (delay > SIZE_MAX - simulation::elapsed_time) // overflow check.
		return; // TODO(Rafael): remove entity that would live longer than sim
		        // time.

	// case inifinite servers (no queue, i.e., only delay is the service
	// duration's):
	if (!server_count)
		simulation::events.emplace(
		    std::piecewise_construct, std::forward_as_tuple(delay),
		    std::forward_as_tuple(std::make_unique<event>(
		        e.ent, outlet, simulation::elapsed_time, delay)));

	// case finite servers (single queue, i.e., there is a queue-wait delay and
	// a service-duration delay):
	else
	{
		entity available_server = get_next_available_server();
		simulation::events.emplace(
		    std::piecewise_construct, std::forward_as_tuple(delay),
		    std::forward_as_tuple(std::make_unique<event>(
		        e.ent, outlet, simulation::elapsed_time, delay)));
		// update_statistics();
		servers[available_server] += delay;
	}
}

entity server::get_next_available_server() const
{
	entity s = 0;
	for (entity i = 0; i < server_count; ++i)
		if (servers[i] < servers[s])
			s = i;
	return s;
}

time_unit server::sample_from_distribution()
{
	time_unit diff_time = upper_service_time - lower_service_time;
	time_unit delay = std::rand() % lower_service_time + diff_time;
	return delay;
}

using json = nlohmann::json;

void to_json(json &j, const server &s)
{
	j = json {{"id", s.id},
	          {"name", s.name},
	          {"type", "server"},
	          {"servers", s.server_count},
	          {"lower_service_time", s.lower_service_time},
	          {"upper_service_time", s.upper_service_time},
	          {"outlet", s.outlet->id}};
}

void from_json(const json &j, server &s)
{
	SSFW_ASSERT(std::string(j.at("type")).compare("server"),
	            "Attempt to read non-server JSON as a server object.");

	j.at("id").get_to(s.id);
	j.at("name").get_to(s.name);
	j.at("servers").get_to(s.server_count);
	j.at("lower_service_time").get_to(s.lower_service_time);
	j.at("lower_service_time").get_to(s.lower_service_time);
	j.at("outlet").get_to(s.outlet->id);
}

} // namespace ssfw