#pragma once
#include <string>

namespace Engine::Logger {
void Info(const std::string &message);
void Warning(const std::string &message);
void Error(const std::string &message);
void Fatal(const std::string &message, const char *file, int line);
} // namespace Engine::Logger

#if defined(__GNUC__) || defined(__clang__)
#define ENGINE_DEBUG_BREAK() __builtin_trap()
#elif defined(_MSC_VER)
#define ENGINE_DEBUG_BREAK() __debugbreak()
#else
#define ENGINENGINE_DEBUG_BREAK()
#endif

#define ENGINE_FATAL(message)                                                  \
  {                                                                            \
    Engine::Logger::Fatal(message, __FILE__, __LINE__);                        \
    ENGINE_DEBUG_BREAK();                                                      \
  }
