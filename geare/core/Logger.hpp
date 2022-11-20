#ifndef _INCLUDE__GEARE__CORE__LOGGER_
#define _INCLUDE__GEARE__CORE__LOGGER_

#include "../utils/Singleton.hpp"
#include <iostream>
#include <stack>
#include <vector>

namespace geare::core {

enum struct LogSeverity : unsigned char {
  Debug,
  Info,
  Warning,
  Error,
  Critical,
  count,
};

struct LoggerContextStack : utils::PerThreadSingleton<LoggerContextStack> {
  std::stack<std::string> contexts;

  std::string_view get_current_context() const noexcept {
    if (!contexts.empty()) {
      return contexts.top();
    } else {
      return "global";
    }
  }
};

void log_begin_ctx(std::string_view name) {
  LoggerContextStack::instance().contexts.push(std::string(name));
}

void log_end_ctx() { LoggerContextStack::instance().contexts.pop(); }

struct Logger : utils::Singleton<Logger> {
  std::vector<std::tuple<LogSeverity, LogSeverity, std::ostream *>> outputs;

  Logger() {
    outputs.push_back({LogSeverity::Debug, LogSeverity::Critical, &std::cout});
    outputs.push_back({LogSeverity::Error, LogSeverity::Critical, &std::cerr});
  }

  static const char *log_severity_to_cstr(LogSeverity severity) {
    if (severity >= LogSeverity::count)
      return nullptr;
    static const char *severities[] = {"DEBUG", "INFO", "WARNING", "ERROR",
                                       "CRITICAL"};
    return severities[(std::size_t)severity];
  }

  template <typename... Args> void log(LogSeverity severity, Args... args) {
    for (auto [severity1, severity2, output] : outputs) {
      if (severity1 > severity2)
        std::swap(severity1, severity2);

      if (severity1 <= severity && severity <= severity2) {
        *output << '[' << log_severity_to_cstr(severity) << " @ "
                << LoggerContextStack::instance().get_current_context() << "] ";
        (*output << ... << args) << std::endl;
      }
    }
  }
};

template <typename... Args> void log_dbg(Args... args) {
  return geare::core::Logger::instance().log(geare::core::LogSeverity::Debug,
                                             args...);
}
template <typename... Args> void log_info(Args... args) {
  return geare::core::Logger::instance().log(geare::core::LogSeverity::Info,
                                             args...);
}
template <typename... Args> void log_warn(Args... args) {
  return geare::core::Logger::instance().log(geare::core::LogSeverity::Warning,
                                             args...);
}
template <typename... Args> void log_err(Args... args) {
  return geare::core::Logger::instance().log(geare::core::LogSeverity::Error,
                                             args...);
}

template <typename... Args> void log_crit(Args... args) {
  return geare::core::Logger::instance().log(geare::core::LogSeverity::Critical,
                                             args...);
}

} // namespace geare::core

namespace geare {

using core::log_begin_ctx;
using core::log_crit;
using core::log_dbg;
using core::log_end_ctx;
using core::log_err;
using core::log_info;
using core::log_warn;

} // namespace geare

#endif
