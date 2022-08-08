#pragma once

#include "component.h"
#include "core.h"

namespace ssfw
{

class Server : public Component
{
public:
	struct Statistics
	{
		time_unit idleness;
		time_unit avg_wait;
		time_unit avg_delay;
		entity avg_queue_size;
	};

	Statistics statistics {};
	time_unit lower_service_time;
	time_unit upper_service_time;
	entity server_count;
	time_unit *servers;

	Server(const std::string name, uint8_t id, Component *outlet,
	       time_unit lower_service_time, time_unit upper_service_time,
	       entity servers)
	    : Component(name, id, outlet), lower_service_time(lower_service_time),
	      upper_service_time(upper_service_time), server_count(servers),
	      servers(new time_unit[servers])
	{
		for (entity i = 0; i < server_count; ++i)
			this->servers[i] = 0;
	}

	Server(const char *name, uint8_t id, Component *outlet,
	       time_unit lower_service_time, time_unit upper_service_time,
	       entity servers)
	    : Component(name, id, outlet), lower_service_time(lower_service_time),
	      upper_service_time(upper_service_time), server_count(servers),
	      servers(new time_unit[servers])
	{
		for (entity i = 0; i < server_count; ++i)
			this->servers[i] = 0;
	}

	Server() = default;

	~Server() { delete[] servers; }

	entity get_next_available_server() const;
	virtual void evaluate_event(Event &e) override;
	virtual void update_statistics(Event &e) override;
	virtual time_unit sample_from_distribution() override;

	using json = nlohmann::json;
	friend void to_json(json &j, const Server &s);
	friend void from_json(const json &j, Server &s);
};

} // namespace ssfw