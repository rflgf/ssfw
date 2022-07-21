#include "sink.h"

namespace ssfw
{

void sink::evaluate_event(event &e) { update_statistics(e); }

void sink::update_statistics(event &e) {}

time_unit sink::sample_from_distribution() { return 0; }

using json = nlohmann::json;

void to_json(json &j, const ssfw::sink &s)
{
	j = json {{"id", s.id}, {"name", s.name}, {"type", "sink"}};
}

void from_json(const json &j, ssfw::sink &s)
{
	SSFW_ASSERT(std::string(j.at("type")).compare("sink"),
	            "Attempt to read non-sink JSON as a sink object.");

	j.at("id").get_to(s.id);
	j.at("name").get_to(s.name);
}

} // namespace ssfw