#ifndef LOG_H
#define LOG_H

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

class Logger {
public:
  static std::shared_ptr<spdlog::logger> &get_logger() { return s_logger; }
  static void init() {
    s_logger = spdlog::stdout_color_mt("logger");
    s_logger->set_level(spdlog::level::trace);
  }

private:
  static std::shared_ptr<spdlog::logger> s_logger;
};

#define MC_FATAL(...) Logger::get_logger()->critical(__VA_ARGS__)
#define MC_ERROR(...) Logger::get_logger()->error(__VA_ARGS__)

#if defined(MC_DEBUG_BUILD)

#define MC_WARN(...) Logger::get_logger()->warn(__VA_ARGS__)
#define MC_DEBUG(...) Logger::get_logger()->debug(__VA_ARGS__)
#define MC_INFO(...) Logger::get_logger()->info(__VA_ARGS__)

#else

#define MC_WARN(...) void(0)
#define MC_DEBUG(...) void(0)
#define MC_INFO(...) void(0)

#endif

#endif
