#pragma once

#include "component.h"
#include "core.h"

namespace ssfw
{

class Sink : public Component
{
public:
	struct Statistics
	{
		entity entity_count;
		time_unit avg_entity_lifetime;
	};

	Statistics statistics;

	Sink(const std::string name, uint8_t id) : Component(name, id, nullptr) {}

	Sink(const char *name, uint8_t id) : Component(name, id, nullptr) {}

	Sink() = default;

	virtual void evaluate_event(Event &e) override;
	virtual void update_statistics(Event &e) override;
	virtual time_unit sample_from_distribution() override;

	using json = nlohmann::json;
	friend void to_json(json &j, const Sink &s);
	friend void from_json(const json &j, Sink &s);
};

} // namespace ssfw
