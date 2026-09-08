#include "util.h"

#include <filesystem>
#include <mach-o/dyld.h>
#include <string>

std::filesystem::path get_executable_path() {
  uint32_t size = 0;
  _NSGetExecutablePath(nullptr, &size);

  std::string buffer(size, '\0');
  _NSGetExecutablePath(buffer.data(), &size);

  return std::filesystem::path(buffer).parent_path();
}

std::filesystem::path get_resource_path() {
#ifdef MINECRAFT_RESOURCE_DIR
  return std::filesystem::path(MINECRAFT_RESOURCE_DIR);
#else
  return get_executable_path() / "Resources";
#endif
}
