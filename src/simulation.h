#pragma once

#include "core.h"
#include "event.h"
#include "model.h"

namespace ssfw
{
namespace simulation
{

void run(Model &m);

void evaluate_event(Event &e, time_unit elapsed_time);

} // namespace simulation
} // namespace ssfw
