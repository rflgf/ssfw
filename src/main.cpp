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

	// CALCULATE OUTPUT
	// FREE COMPONENTS AND MODEL
}
