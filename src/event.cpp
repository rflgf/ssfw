#include "event.h"
#include "components/component.h"

namespace ssfw
{

std::ostream &operator<<(std::ostream &stream, const event &e)
{
	if (!e.comp)
		return stream;

	stream << "event: entity " << e.ent << " at component " << e.comp->name
	       << ", with start " << e.start << " and duration " << e.duration
	       << ", with end " << e.start + e.duration;
	return stream;
}

} // namespace ssfw
