#include "model.h"
#include "simulation.h"

int main()
{
	// READ FILE INTO MODEL
	ssfw::model model("Exemplo", 20);
	model.load_from("example.json");

	// VALIDATE MODEL
	model.validate();

	// RUN SIMULATION
	ssfw::run(model);

	// CALCULATE OUTPUT
	// FREE COMPONENTS AND MODEL
}
