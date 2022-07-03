#pragma once

#include "core.h"
#include "component.h"

namespace ssfw {

	class output : public component
	{
	public:
		struct statistics
		{
			entity entity_count;
			time_unit avg_entity_lifetime;
		};

		statistics statistics; // is this fine lol

		virtual void update_statistics(event &e) override;
	};
}
