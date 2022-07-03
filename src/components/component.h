#pragma once

#include "core.h"

namespace ssfw {

		class component
	{
	public:
		
		component(std::string name, uint32_t id, component* outlet)
			: name(name), id(id), outlet(outlet) {}

		component(const char* name, uint32_t id, component* outlet)
			: name(name), id(id), outlet(outlet) {}
		
		std::string name;
		uint8_t id;
		component* outlet;

		virtual void update_statistics(event& e) = 0;
	};
}
