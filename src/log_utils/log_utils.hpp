//
// Created by zxsrxt on 10.03.2026.
//

#ifndef BSTK_ROOT_PATCHER_LOG_UTILS_HPP
#define BSTK_ROOT_PATCHER_LOG_UTILS_HPP

#include <iostream>
#include <string>
#include <format>
#include <chrono>
#include <mutex>

#include "сonsole_ui.hpp"

namespace logger
{
    enum class level
    {
        info,
        warn,
        error,
        debug
    };

    namespace detail
    {
        inline std::mutex log_mutex;

        inline std::string level_to_string(level lvl)
        {
            switch (lvl)
            {
                case level::info:  return "info";
                case level::warn:  return "warn";
                case level::error: return "error";
                case level::debug: return "debug";
                default:           return "unknown";
            }
        }

        inline std::string current_time()
        {
            using namespace std::chrono;

            auto now = system_clock::now();
            auto time = system_clock::to_time_t(now);

            std::tm tm{};
#ifdef _WIN32
            localtime_s(&tm, &time);
#else
            localtime_r(&time, &tm);
#endif
            return std::format("{:02}:{:02}:{:02}",
                               tm.tm_hour,
                               tm.tm_min,
                               tm.tm_sec);
        }
    }

    template<typename... Args>
    void log(level lvl, std::format_string<Args...> fmt, Args&&... args)
    {
        std::lock_guard lock(detail::log_mutex);

        console_ui ui;

        std::string message = std::format(fmt, std::forward<Args>(args)...);
        std::string time = detail::current_time();
        std::string lvl_str = detail::level_to_string(lvl);

        std::string final_str = std::format("[{} {}] {}", lvl_str, time, message);

        switch (lvl)
        {
            case level::warn:
            case level::debug:
            case level::info:
                ui.set_color(Color::Cyan);
                std::cout << final_str << std::endl;
                ui.reset_color();
                break;
            case level::error:
                ui.set_color(Color::Red);
                std::cerr << final_str << std::endl;
                ui.reset_color();
                break;
        }
    }

    template<typename... Args>
    void log_info(std::format_string<Args...> fmt, Args&&... args)
    {
        log(level::info, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void log_warn(std::format_string<Args...> fmt, Args&&... args)
    {
        log(level::warn, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void log_error(std::format_string<Args...> fmt, Args&&... args)
    {
        log(level::error, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void log_debug(std::format_string<Args...> fmt, Args&&... args)
    {
        log(level::debug, fmt, std::forward<Args>(args)...);
    }

} // namespace logger


#endif //BSTK_ROOT_PATCHER_LOG_UTILS_HPP