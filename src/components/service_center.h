#pragma once

#include "core.h"
#include "component.h"

namespace ssfw {

	class service_center : public component
	{
	public:
		struct statistics
		{
			time_unit idleness;
			time_unit avg_wait;
			time_unit avg_delay;
			time_unit avg_queue_size; // should this really be time_unit tho
		};

		statistics statistics;

		virtual void update_statistics(event &e) override;
	};
}
