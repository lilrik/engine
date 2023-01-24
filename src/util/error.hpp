#pragma once
#include <iostream>

namespace util {
static void panicIfCond(auto condition, const char *message) {
  if (!condition) {
    std::cerr << "error: " << message << std::endl;
    exit(1);
  }
}

[[maybe_unused]] static void panicIfErr(int err, const char *message) {
  if (err) {
    std::cerr << "error: " << err << " " << message << std::endl;
    exit(1);
  }
}
} // namespace util
