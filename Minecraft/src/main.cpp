#include "Log.h"
#include "Minecraft.h"

int main() {

  Logger::init();
  MC_DEBUG("Initialized Logger");

  MC_INFO("Starting ...");
  Minecraft app;
  app.run();

  MC_INFO("Shutting Down ...");
}
