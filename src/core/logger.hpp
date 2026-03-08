#pragma once
#include <iostream>
#include <string>

namespace Engine::Logger {
    inline void Info(const std::string &message) {
        std::cout << "[INFO]: " << message << std::endl;
    }

    inline void Warning(const std::string &message) {
        std::cout << "[WARNING]: " << message << std::endl;
    }

    inline void Error(const std::string &message) {
        std::cerr << "[ERROR]: " << message << std::endl;
    }

    inline void Fatal(const std::string &message, const char *file, int line) {
        std::cerr << "[FATAL] (" << file << ":" << line << "): " << message
                  << std::endl;
    }
} // namespace Engine::Logger

#if defined(__GNUC__) || defined(__clang__)
    #define ENGINE_DEBUG_BREAK() __builtin_trap()
#elif defined(_MSC_VER)
    #define ENGINE_DEBUG_BREAK() __debugbreak()
#else
    #define ENGINE_DEBUG_BREAK()
#endif

#define ENGINE_INFO(message) Engine::Logger::Info(message)
#define ENGINE_WARN(message) Engine::Logger::Warning(message)
#define ENGINE_ERROR(message)                                                  \
    Enginer:                                                                   \
    Logger::Error(message)
#define ENGINE_FATAL(message)                                                  \
    {                                                                          \
        Engine::Logger::Fatal(message, __FILE__, __LINE__);                    \
        ENGINE_DEBUG_BREAK();                                                  \
    }
