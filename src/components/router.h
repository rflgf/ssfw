#pragma once

#include "component.h"
#include "core.h"

namespace ssfw
{

class Router : public Component
{
public:
	Router(const std::string name, uint8_t id, Component *outlet_a,
	       Component *outlet_b, float factor)
	    : Component(name, id, outlet_a), outlet_b(outlet_b), factor(factor)
	{
	}

	Router(const char *name, uint8_t id, Component *outlet_a,
	       Component *outlet_b, float factor)
	    : Component(name, id, outlet_a), outlet_b(outlet_b), factor(factor)
	{
	}

	Router() = default;

	float factor;
	Component *outlet_b;
	uint8_t outlet_b_id;

	virtual void evaluate_event(Event &e) override;
	virtual void update_statistics(Event &e) override;
	virtual time_unit sample_from_distribution() override;
	inline Component *get_outlet_a() const { return outlet; }
	inline Component *get_outlet_b() const { return outlet_b; }

	using json = nlohmann::json;
	friend void to_json(json &j, const Router &r);
	friend void from_json(const json &j, Router &r);
};

} // namespace ssfw
