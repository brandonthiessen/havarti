#pragma once

#include <iostream>
#include <format>

enum class LogLevel {
    OFF = 0,
    ERROR = 1,
    INFO = 2,
    DEBUG = 3
};

class Logger {
public:
    inline static LogLevel level = LogLevel::INFO;
};

#define LOG_ERROR(fmt, ...) \
    do { if (Logger::level >= LogLevel::ERROR) \
        std::cout << std::format("[ERROR] " fmt "\n", __VA_ARGS__); \
    } while(0)

#define LOG_INFO(fmt, ...) \
    do { if (Logger::level >= LogLevel::INFO) \
        std::cout << std::format("[INFO] " fmt "\n", __VA_ARGS__); \
    } while(0)

#define LOG_DEBUG(fmt, ...) \
    do { if (Logger::level >= LogLevel::DEBUG) \
        std::cout << std::format("[DEBUG] " fmt "\n", __VA_ARGS__); \
    } while(0)
