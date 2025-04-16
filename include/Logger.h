#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <ctime>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <unistd.h>

#if defined(_WIN32)
#include <windows.h>
#endif

class Logger {
public:
    enum class Level { Debug, Info, Warning, Error };

    struct Config {
        bool showTimestamp;
        bool showLevel;
        bool showSourceLocation;
        bool enableColors;

        constexpr Config()
            : showTimestamp(false),
              showLevel(true),
              showSourceLocation(true),
              enableColors(true) {}

        explicit constexpr Config(
            const bool timestamp,
            const bool level,
            const bool source,
            const bool color)
            : showTimestamp(timestamp),
              showLevel(level),
              showSourceLocation(source),
              enableColors(color) {}
    };

    static void setConfig(const Config& cfg) {
        std::lock_guard lock(mtx);
        m_config = cfg;
    }

    template <typename... Args>
    static void debug(const std::string& file, int line, const std::string& func, Args&&... args) {
        log(Level::Debug, file, line, func, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void info(const std::string& file, int line, const std::string& func, Args&&... args) {
        log(Level::Info, file, line, func, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void warn(const std::string& file, int line, const std::string& func, Args&&... args) {
        log(Level::Warning, file, line, func, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void error(const std::string& file, int line, const std::string& func, Args&&... args) {
        log(Level::Error, file, line, func, std::forward<Args>(args)...);
    }

private:
    static inline std::mutex mtx;
    static inline Config m_config{};

    static const char* levelToStr(const Level level) {
        switch (level) {
            case Level::Debug:   return "DEBUG";
            case Level::Info:    return "INFO";
            case Level::Warning: return "WARNING";
            case Level::Error:   return "ERROR";
        }
        return "UNKNOWN";
    }

    static const char* levelToColor(const Level level) {
        if (!shouldUseColors()) return "";
        switch (level) {
            case Level::Debug:   return "\033[38;5;44m";   // Teal Cyan
            case Level::Info:    return "\033[38;5;75m";   // Sky Blue
            case Level::Warning: return "\033[38;5;214m";  // Amber
            case Level::Error:   return "\033[38;5;203m";  // Soft Red
        }
        return "\033[0m";
    }

    static std::string formatSourceLocation(const std::string& file, const int line, const std::string& func) {
        const size_t lastSlash = file.find_last_of("/\\");
        const std::string filename = (lastSlash == std::string::npos)
            ? file
            : file.substr(lastSlash + 1);

        return filename + ":" + std::to_string(line);
    }

    static std::tm localTime(std::time_t time) {
        std::tm tm{};
        #if defined(_WIN32)
            localtime_s(&tm, &time);
        #else
            localtime_r(&time, &tm);
        #endif
        return tm;
    }

    static std::string currentTimestamp() {
        using namespace std::chrono;
        const auto now = system_clock::now();
        const std::time_t time = system_clock::to_time_t(now);
        const std::tm tm = localTime(time);

        char buffer[16];
        std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &tm);
        return {buffer};
    }

    static bool shouldUseColors() {
        #if defined(_WIN32)
            static bool isTerminal = []() {
                HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
                if (hOut == INVALID_HANDLE_VALUE) return false;
                DWORD mode = 0;
                return GetConsoleMode(hOut, &mode);
            }();
            return isTerminal && m_config.enableColors;
        #else
            return m_config.enableColors && isatty(fileno(stdout));
        #endif
    }

    template <typename... Args>
    static void log(const Level level, const std::string& file, const int line, const std::string& func, Args&&... args) {
        std::ostringstream oss;

        if (m_config.showTimestamp) {
            const std::string timestamp = "[" + currentTimestamp() + "] ";
            oss << timestamp;
        }

        const std::string colorStart = levelToColor(level);
        const std::string colorEnd = shouldUseColors() ? "\033[0m" : "";

        if (m_config.showSourceLocation) {
            oss << "[" << formatSourceLocation(file, line, func) << "] ";
        }

        if (m_config.showLevel) {
            oss << colorStart << "[" << levelToStr(level) << "]" << " ";
        }

        (oss << ... << std::forward<Args>(args));

        const std::string message = oss.str();

        std::lock_guard lock(mtx);
        std::cout << message << "\n" << colorEnd;
    }
};

// Macros to capture source location
#define DEBUG(...) Logger::debug(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define INFO(...)  Logger::info(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define WARN(...)  Logger::warn(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define ERROR(...) Logger::error(__FILE__, __LINE__, __func__, __VA_ARGS__)

#endif // LOGGER_H