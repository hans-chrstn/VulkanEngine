#pragma once
#include <string>

namespace Engine::Logger {
void Info(const std::string &message);
void Warning(const std::string &message);
void Error(const std::string &message);
} // namespace Engine::Logger
