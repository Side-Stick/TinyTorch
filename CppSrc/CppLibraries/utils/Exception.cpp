#include "Exception.hpp"

#include <utility>

Exception::Exception(std::string msg, std::string traceback_source) {
    msg_ = std::move(msg);
    trace_track_.push_back(std::move(traceback_source));
}

void Exception::add_track(std::string trace) {
    trace_track_.push_back(std::move(trace));
}

TensorWarning::TensorWarning(TensorWarningCode warning_type_code,
                             std::string msg,
                             std::string traceback_source)
        : Exception(std::move(msg), std::move(traceback_source)),
          type_code_(warning_type_code) {}
