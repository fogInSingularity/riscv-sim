#ifndef TRACE_CALLS_HPP_
#define TRACE_CALLS_HPP_

#include <source_location>

#include <spdlog/spdlog.h>

namespace hlp {

inline void trace_call(std::source_location loc = std::source_location::current()) {
    // spdlog::debug("Function call: {}, {}:{}", loc.function_name(), loc.file_name(), loc.line());
    spdlog::debug("Function call: {}", loc.function_name());
}

} // namespace hlp

#endif // TRACE_CALLS_HPP_

