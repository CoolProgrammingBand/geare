#ifndef _INCLUDE__GEARE__CORE__LOGGER_
#define _INCLUDE__GEARE__CORE__LOGGER_

#include "../utils/Singleton.hpp"
#include <iostream>
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

struct Logger : utils::Singleton<Logger> {
  std::vector<std::tuple<LogSeverity, LogSeverity, std::ostream *>> outputs;

  Logger() {
    outputs.push_back({LogSeverity::Debug, LogSeverity::Critical, &std::cout});
  }

  static const char *log_severity_to_cstr(LogSeverity severity) {
    if (severity >= LogSeverity::count)
      return nullptr;
    static const char *severities[] = {"DEBUG", "INFO", "WARNING", "ERROR",
                                       "CRITICAL"};
    return severities[(std::size_t)severity];
  }

  template <typename... Args>
  void log(std::string_view context, LogSeverity severity, Args... args) {
    for (auto [severity1, severity2, output] : outputs) {
      if (severity1 > severity2)
        std::swap(severity1, severity2);

      if (severity1 <= severity && severity <= severity2) {
        *output << '[' << log_severity_to_cstr(severity) << " @ " << context
                << "] ";
        (*output << ... << args) << std::endl;
      }
    }
  }
};

} // namespace geare::core

#endif
