#include "logger.hpp"
#include <iostream>
#include <ostream>
#include <stdexcept>

namespace Engine::Logger {
void Info(const std::string &message) {
  std::cout << "[INFO]: " << message << std::endl;
}

void Warning(const std::string &message) {
  std::cout << "[WARNING]: " << message << std::endl;
}

void Error(const std::string &message) {
  std::cerr << "[ERROR]: " << message << std::endl;
}

void Fatal(const std::string &message, const char *file, int line) {
  std::cerr << "[FATAL] (" << file << ":" << line << "): " << message
            << std::endl;
}
} // namespace Engine::Logger
