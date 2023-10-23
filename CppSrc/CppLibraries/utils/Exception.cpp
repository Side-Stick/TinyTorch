#include "Exception.hpp"

Exception::Exception(std::string msg, std::string traceback_source) {
    msg_ = std::move(msg);
    trace_track_.push_back(std::move(traceback_source));
}

void Exception::add_track(std::string trace) {
    trace_track_.push_back(std::move(trace));
}
