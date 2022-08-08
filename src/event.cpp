#include "event.h"
#include "components/component.h"

namespace ssfw
{

std::ostream &operator<<(std::ostream &stream, const Event &e)
{
	if (!e.component)
		return stream;

	stream << "event: entity " << e.ent << " at component " << e.component->name
	       << ", with start " << e.start << " and duration " << e.duration
	       << ", with end " << e.start + e.duration;
	return stream;
}

} // namespace ssfw
