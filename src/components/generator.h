#pragma once

#include "component.h"
#include "core.h"

namespace ssfw
{

class generator : public component
{
public:
	generator(const std::string name, uint8_t id, component *outlet, entity l,
	          entity u)
	    : component(name, id, outlet), lower_throughput(l), upper_throughput(u)
	{
	}

	generator(const char *name, uint8_t id, component *outlet, entity l,
	          entity u)
	    : component(name, id, outlet), lower_throughput(l), upper_throughput(u)
	{
	}

	generator() = default;

	entity lower_throughput;
	entity upper_throughput;

	virtual void evaluate_event(event &e) override;
	virtual void update_statistics(event &e) override;
	virtual time_unit sample_from_distribution() override;

	using json = nlohmann::json;
	friend void to_json(json &j, const generator &g);
	friend void from_json(const json &j, generator &g);
};

} // namespace ssfw