#ifndef TINYTORCHCPPSRC_INTRUSIVE_PTR_HPP
#define TINYTORCHCPPSRC_INTRUSIVE_PTR_HPP

#include <atomic>

namespace t_tensor {

class intrusive_ptr_target {
  private:
    mutable std::atomic<uint32_t> ref_count_;
    mutable std::atomic<uint32_t> weak_count_;
};

} //namespace t_tensor

#endif //TINYTORCHCPPSRC_INTRUSIVE_PTR_HPP
