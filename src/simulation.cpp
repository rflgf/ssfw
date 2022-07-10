#include "simulation.h"

#include <cstdlib>

namespace ssfw
{
namespace simulation
{
void print_events(const std::multimap<time_unit, std::unique_ptr<event>> &m);

std::multimap<time_unit, std::unique_ptr<event>> events;
time_unit elapsed_time = 0;

void run(model &m)
{
	entity entities = 0;

	for (auto gen : m.get_generators())
	{
		elapsed_time = 0;
		time_unit diff_time = gen->upper_throughput - gen->lower_throughput;

		while (true)
		{
			if (elapsed_time > m.sim_time_limit || entities == SIZE_MAX)
				break;

			time_unit spawn_timestamp = (std::rand() % diff_time) +
			                            gen->lower_throughput + elapsed_time;
			if (spawn_timestamp > SIZE_MAX - elapsed_time) // overflow check.
				break;

			events.emplace(std::piecewise_construct,
			               std::forward_as_tuple(spawn_timestamp),
			               std::forward_as_tuple(std::make_unique<event>(
			                   entities++, gen->outlet, spawn_timestamp,
			                   gen->outlet->sample_from_distribution())));

			elapsed_time = spawn_timestamp;
		}
	}

	time_unit elapsed_time = 0;

	print_events(events);

	while (elapsed_time <= m.sim_time_limit && !events.empty())
	{
		elapsed_time = events.begin()->first;

		auto events_in_current_ts = events.equal_range(elapsed_time);

		for (auto timestamp_event_pair = events_in_current_ts.first;
		     timestamp_event_pair != events_in_current_ts.second;
		     ++timestamp_event_pair)
		{
			std::unique_ptr<event> &e = timestamp_event_pair->second;
			e->comp->evaluate_event(*e.get());

			std::cout << *e.get() << "\n";
		}

		events.erase(elapsed_time);

		print_events(events);
	}
}

void print_events(const std::multimap<time_unit, std::unique_ptr<event>> &m)
{
	std::cout << "printing events:"
	          << "\n{\n";
	for (const auto &i : m)
		std::cout << '\t' << *i.second.get() << '\n';
	std::cout << "}\n";
}

} // namespace simulation
} // namespace ssfw
