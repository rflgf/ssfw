#pragma once

#include "core.h"
#include "component.h"

namespace ssfw {

	class router : public component
	{
	public:
		router(std::string name, uint8_t id, component* out_a, component* out_b, float factor)
			: component(name, id, out_a), out_b(out_b), factor(factor) {}

		float factor;
		component* out_b;

		virtual void update_statistics(event& e) override;
		inline component* get_outlet_a() const { return outlet; }
		inline component* get_outlet_b() const { return out_b;  }
	};
}
