#include "util.h"

#include <filesystem>
#include <mach-o/dyld.h>
#include <string>

std::filesystem::path getExecutablePath() {
  uint32_t size = 0;
  _NSGetExecutablePath(nullptr, &size);

  std::string buffer(size, '\0');
  _NSGetExecutablePath(buffer.data(), &size);

  return std::filesystem::path(buffer).parent_path();
}
