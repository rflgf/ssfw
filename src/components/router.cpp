#include "router.h"
#include <cstdlib>

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

} // namespace ssfw
