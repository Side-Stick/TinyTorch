#ifndef TINYTORCHCPPSRC_EXCEPTION_HPP
#define TINYTORCHCPPSRC_EXCEPTION_HPP

#include <exception>
#include <string>
#include <vector>

class Exception : public std::exception {
    std::string msg_;
    std::vector<std::string> trace_track_;

  public:
    Exception(std::string msg, std::string traceback_source);
    void add_track(std::string trace);
};

class TensorWarning : public Exception {
  public:
    enum TensorWarningCode {
        REF_COUNT_WARN,
    };

    TensorWarning(TensorWarningCode warning_type_code, std::string msg,
                  std::string traceback_source);

  private:
    TensorWarningCode type_code_;
};

#endif //TINYTORCHCPPSRC_EXCEPTION_HPP
