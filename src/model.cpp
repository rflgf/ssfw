#include "model.h"

#include "components/generator.h"
#include "components/router.h"
#include "components/server.h"
#include "components/sink.h"

namespace ssfw
{

model::model(const char *name, time_unit sim_time_limit)
    : name(name), sim_time_limit(sim_time_limit)
{
	// hardcoding componentes por hora.
	sink *s1 = new sink("Saída A", 3);
	sink *s2 = new sink("Saída B", 4);
	router *r = new router("Roteador A", 2, s1, s2, 0.3f);
	server *s = new server("Centro de Serviço A", 1, r, 4, 5, 3);
	generator *g = new generator("Entrada A", 0, s, 1, 3);

	components.push_front(g);
	generators.push_front(g);
	components.push_front(s);
	components.push_front(r);
	components.push_front(s1);
	components.push_front(s2);
}

model::~model()
{
	for (auto c : components)
		delete c;
}

model model::load_from(std::string_view file_path)
{
	return model("Placeholder", 10000);
}

void model::validate() {}

void model::update_statistics(event &e) {}

} // namespace ssfw
