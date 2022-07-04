#pragma once

#include "component.h"
#include "core.h"

namespace ssfw
{

class server : public component
{
public:
	struct statistics
	{
		time_unit idleness;
		time_unit avg_wait;
		time_unit avg_delay;
		entity avg_queue_size;
	};

	statistics statistics;
	time_unit lower_service_time;
	time_unit upper_service_time;
	entity attendants;

	server(const std::string &name, uint8_t id, component *outlet,
	       time_unit lower_service_time, time_unit upper_service_time,
	       entity attendants)
	    : component(name, id, outlet), lower_service_time(lower_service_time),
	      upper_service_time(upper_service_time), attendants(attendants)
	{
	}

	virtual void update_statistics(event &e) override;
};

} // namespace ssfw
