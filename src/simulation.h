#pragma once

#include "core.h"
#include "event.h"
#include "model.h"

namespace ssfw
{
namespace simulation
{

void run(model &m);

void evaluate_event(event &e, time_unit elapsed_time);

} // namespace simulation
} // namespace ssfw
