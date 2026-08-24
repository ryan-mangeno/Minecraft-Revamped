#include "Minecraft.h"
#include "Log.h"

int main() {

	Logger::init();

	Minecraft app;
	app.Run();
}
