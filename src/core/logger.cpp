#include "logger.hpp"
#include <iostream>
#include <ostream>

namespace Engine::Logger {
void Info(const std::string &message) {
  std::cout << "[INFO]: " << message << std::endl;
}

void Warning(const std::string &message) {
  std::cout << "[WARNING]: " << message << std::endl;
}

void Error(const std::string &message) {
  std::cerr << "ERROR: " << message << std::endl;
}
} // namespace Engine::Logger
