#pragma once

#include "component.h"
#include "core.h"

namespace ssfw
{

class router : public component
{
public:
	router(const std::string name, uint8_t id, component *outlet_a,
	       component *outlet_b, float factor)
	    : component(name, id, outlet_a), outlet_b(outlet_b), factor(factor)
	{
	}

	router(const char *name, uint8_t id, component *outlet_a,
	       component *outlet_b, float factor)
	    : component(name, id, outlet_a), outlet_b(outlet_b), factor(factor)
	{
	}

	router() = default;

	float factor;
	component *outlet_b;
	uint8_t outlet_b_id;

	virtual void evaluate_event(event &e) override;
	virtual void update_statistics(event &e) override;
	virtual time_unit sample_from_distribution() override;
	inline component *get_outlet_a() const { return outlet; }
	inline component *get_outlet_b() const { return outlet_b; }

	using json = nlohmann::json;
	friend void to_json(json &j, const router &r);
	friend void from_json(const json &j, router &r);
};

} // namespace ssfw
