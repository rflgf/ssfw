#include "generator.h"

namespace ssfw
{

void generator::evaluate_event(event &e) { e.comp->evaluate_event(e); }

void generator::update_statistics(event &e) {}

time_unit generator::sample_from_distribution() { return 0; }

} // namespace ssfw
