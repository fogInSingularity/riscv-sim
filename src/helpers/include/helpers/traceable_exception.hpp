#ifndef TRACEABLE_EXCEPTION_HPP_
#define TRACEABLE_EXCEPTION_HPP_

#include <cstdlib>
#include <stdexcept>
#include <string>

#include <execinfo.h>
#include <dlfcn.h>
#include <cxxabi.h>

#include <spdlog/spdlog.h>

namespace hlp {

class TraceableException : public std::runtime_error {
  private:
    void dump_trace() {
        constexpr int skip = 3;
        constexpr int trace_cap = 32;
        void* trace_buffer[trace_cap] = {};

        int trace_size = backtrace(trace_buffer, trace_cap); 
        char** trace = backtrace_symbols(trace_buffer, trace_size);
        if (trace == nullptr) {
            spdlog::debug("Unable to trace");
        }
    
        spdlog::debug("trace:");
        for (int i = skip; i < trace_size; i++) {
            Dl_info info{};

            if (dladdr(trace_buffer[i], &info) && info.dli_sname) {
                char* demangled_trace = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, nullptr);
                spdlog::debug("{}", demangled_trace ? demangled_trace : info.dli_sname);
                free(demangled_trace);
            } else {
                spdlog::debug("{}", trace[i]);
            }
        }

        free(static_cast<void*>(trace));
    }
 
  public:
    explicit TraceableException(const std::string& err_msg, bool generate_trace = true) : std::runtime_error{err_msg}
    {
        spdlog::error(err_msg);
        if (!generate_trace) { return ; }

        dump_trace();       
    }
};

} // namespace hlp

#endif // TRACEABLE_EXCEPTION_HPP_

