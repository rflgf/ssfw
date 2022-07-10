#include "sink.h"

namespace ssfw
{

void sink::evaluate_event(event &e) { update_statistics(e); }

void sink::update_statistics(event &e) {}

time_unit sink::sample_from_distribution() { return 0; }

} // namespace ssfw
