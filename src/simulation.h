#pragma once

#include "core.h"
#include "model.h"
#include "components/component.h"

namespace ssfw {

	void run(model &m);

	void evaluate_event(event &e, time_unit elapsed_time);
}
