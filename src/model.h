#pragma once

#include "components/component.h"
#include "core.h"

namespace ssfw
{

class model
{
public:
	struct statistics
	{
		time_unit avg_entity_lifetime;
	};

	time_unit elapsed_time = 0;
	std::list<component> components;
	std::list<std::reference_wrapper<component>> input_components;

	void load_from(std::string_view file_path);
	void validate();
	void update_statistics(event &e);
	inline std::list<std::reference_wrapper<component>>
	get_input_components() const
	{
		return input_components;
	};
};

} // namespace ssfw
