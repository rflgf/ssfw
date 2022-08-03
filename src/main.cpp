
#include "application.h"
#include "core.h"
#include "model.h"
#include "simulation.h"

int main()
{
	// READ FILE INTO MODEL
	ssfw::model model("Exemplo", 5);
	model.load_from("example.json");

	// VALIDATE MODEL
	model.validate();

	// RUN SIMULATION
	ssfw::simulation::run(model);
	// model.save("example2.json");

	ssfw::application app;
	app.run();
	// app.init();
	// app.main_loop();
	// app.deinit();

	// CALCULATE OUTPUT
	// FREE COMPONENTS AND MODEL
	return 0;
}
