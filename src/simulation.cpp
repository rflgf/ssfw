#include "simulation.h"

#include <cstdlib>

namespace ssfw
{

std::multimap<time_unit, event> events;

void run(model &m)
{
	entity entities = 0;

	for (auto gen : m.get_generators())
	{
		time_unit elapsed_time = 0;
		time_unit diff_time = gen->upper_throughput - gen->lower_throughput;

		while (true)
		{
			if (elapsed_time > m.sim_time_limit || entities == SIZE_MAX)
				break;

			time_unit spawn_timestamp =
			    std::rand() % gen->lower_throughput + diff_time;
			if (spawn_timestamp > SIZE_MAX - elapsed_time) // overflow check.
				break;

			events.emplace(std::piecewise_construct,
			               std::forward_as_tuple(spawn_timestamp),
			               std::forward_as_tuple(entities++, gen->outlet,
			                                     spawn_timestamp, SIZE_MAX));

			elapsed_time += spawn_timestamp;
		}
	}

	time_unit elapsed_time = 0;

	while (elapsed_time <= m.sim_time_limit && !events.empty())
	{
		elapsed_time = events.begin()->first;

		auto events_in_current_ts = events.equal_range(elapsed_time);
		for (auto timestamp_event_pair = events_in_current_ts.first;
		     timestamp_event_pair != events_in_current_ts.second;
		     ++timestamp_event_pair)
		{
			event &e = timestamp_event_pair->second;
			// in-place evaluate event for now:
		}
	}
}

void evaluate_event(event &e, time_unit elapsed_time) {}

} // namespace ssfw
