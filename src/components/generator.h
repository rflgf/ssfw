#pragma once

#include "component.h"
#include "core.h"

namespace ssfw
{

class Generator : public Component
{
public:
	Generator(const std::string name, uint8_t id, Component *outlet, entity l,
	          entity u)
	    : Component(name, id, outlet), lower_throughput(l), upper_throughput(u)
	{
	}

	Generator(const char *name, uint8_t id, Component *outlet, entity l,
	          entity u)
	    : Component(name, id, outlet), lower_throughput(l), upper_throughput(u)
	{
	}

	Generator() = default;

	entity lower_throughput;
	entity upper_throughput;

	virtual void evaluate_event(Event &e) override;
	virtual void update_statistics(Event &e) override;
	virtual time_unit sample_from_distribution() override;

	using json = nlohmann::json;
	friend void to_json(json &j, const Generator &g);
	friend void from_json(const json &j, Generator &g);
};

} // namespace ssfw