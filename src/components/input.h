#pragma once

#include "core.h"
#include "component.h"

namespace ssfw {

	class input : public component
	{
	public:
		input(std::string name, uint8_t id, component* outlet, entity l, entity u)
			: component(name, id, outlet), lower_throughput(l), upper_throughput(u) {}

		entity lower_throughput;
		entity upper_throughput;

		void update_statistics(event& e) override;
	};
}
