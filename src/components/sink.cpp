#include "sink.h"

namespace ssfw
{

void Sink::evaluate_event(Event &e) { update_statistics(e); }

void Sink::update_statistics(Event &e) {}

time_unit Sink::sample_from_distribution() { return 0; }

using json = nlohmann::json;

void to_json(json &j, const Sink &s)
{
	j = json {{"id", s.id}, {"name", s.name}, {"type", "sink"}};
}

void from_json(const json &j, Sink &s)
{
	SSFW_ASSERT(std::string(j.at("type")).compare("sink"),
	            "Attempt to read non-sink JSON as a sink object.");

	j.at("id").get_to(s.id);
	j.at("name").get_to(s.name);
}

} // namespace ssfw