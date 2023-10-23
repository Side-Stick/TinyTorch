#ifndef TINYTORCHCPPSRC_INTRUSIVE_PTR_HPP
#define TINYTORCHCPPSRC_INTRUSIVE_PTR_HPP

#include <atomic>

#include "utils/Exception.hpp"

namespace t_tensor {

class intrusive_ptr_target {
    template<class T> friend class intrusive_ptr;
    template<class T> friend class weak_intrusive_ptr;

  protected:
    mutable std::atomic<std::size_t> ref_count_;
    mutable std::atomic<std::size_t> weak_count_;

    virtual ~intrusive_ptr_target() {
        // An intrusive_ptr_target* should never be destruct directly.
        // The macros below are copied from PyTorch.
#if defined(_MSC_VER) && !defined(__clang__)
        #pragma warning(push)
#pragma warning( \
    disable : 4297) // function assumed not to throw an exception but does
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wterminate"
#pragma GCC diagnostic ignored "-Wexceptions"
#endif
        if (0 != ref_count_.load()) {
            // PyTorch checked if ref_count_.load() >= INT_MAX here
            // as the last line of defence.
            // However, we think the circumstance should be avoided in other
            // code, the condition should never be activated.
            throw Exception(
                    /*msg=*/"Trying to destruct an intrusive_ptr_target with "
                            "refcount_ = "
                            + std::to_string(ref_count_.load())
                            + ", (should be 0)",
                    /*traceback_source=*/"virtual ~intrusive_ptr_target()"
                    );
        }
        if (0 != weak_count_.load()) {
            // PyTorch checked if weak_count_.load() == 1 here
            // because of the optimization in ~intrusive_ptr().
            // We still don't need it yet.
            throw Exception(
                    /*msg=*/"Trying to destruct an intrusive_ptr_target with "
                            "weak_count_ = "
                            + std::to_string(weak_count_.load())
                            + ", (should be 0)",
                    /*traceback_source=*/"virtual ~intrusive_ptr_target()"
            );
        }
#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif
    }

    std::size_t ref_increase_atomic() {
        return ref_count_.fetch_add(1) + 1;
        // PyTorch uses std::memory_order_acq_rel,
        // here we use std::memory_order_seq_cst following the default.
        // If you are not sure which memory order should be selected, then use
        // seq_cst, which is safe but might lead to performance penalties.
        // However, on most X86 platforms, the performance penalty is
        // negligible.

        // fetch_add() returns the value before add, so we need an excessive
        // +1.
    }

    std::size_t weak_increase_atomic() {
        return weak_count_.fetch_add(1) + 1;
    }

    std::size_t ref_decrease_atomic() {
        return ref_count_.fetch_sub(1) - 1;
    }

    std::size_t weak_decrease_atomic() {
        return weak_count_.fetch_sub(1) - 1;
    }
};

template <class TTarget>
class intrusive_ptr final {
  private:
    TTarget* target_;
};

template <class TTarget>
class weak_intrusive_ptr final {

};

} //namespace t_tensor

#endif //TINYTORCHCPPSRC_INTRUSIVE_PTR_HPP
