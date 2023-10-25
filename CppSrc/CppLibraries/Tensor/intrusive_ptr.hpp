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

    // zh: 默认构造函数
    // en: default constructor
    constexpr intrusive_ptr_target() noexcept
            : ref_count_(0), weak_count_(0) {}

    // zh: 移动构造函数
    // en: move constructor
    intrusive_ptr_target(intrusive_ptr_target&& /*others*/) noexcept
            : intrusive_ptr_target() {}
        // zh:
        // typename&& /*others*/是C++ 11的右值引用，能减少一次对象拷贝。
        // 初始化列表的intrusive_ptr_target()调用默认构造函数，
        // 这是为了将两个引用计数清零。其余部分全部“窃取”自被复制的对象。
        // en:
        // typename&& /*others*/ is the rvalue reference, which can reduce
        // an object copy operation.
        // The intrusive_ptr_target() in initialization list calls the
        // default constructor, to set the ref-counters to 0.

    // zh: 移动构造函数
    // en: move constructor
    intrusive_ptr_target(const intrusive_ptr_target& /*other*/) noexcept
            : intrusive_ptr_target() {}

    // zh: 拷贝构造函数
    // en: copy constructor
    intrusive_ptr_target& operator=(intrusive_ptr_target&& /*other*/) noexcept {
        return *this;
    }

    // zh: 拷贝构造函数
    // en: copy constructor
    intrusive_ptr_target& operator=(
            const intrusive_ptr_target& /*other*/) noexcept {
        return *this;
    }

    virtual ~intrusive_ptr_target() {
        // zh: 一个intrusive_ptr_target*不应该被直接析构。
        // en: An intrusive_ptr_target* should never be destruct directly.

        // zh: 以下的宏是从PyTorch复制的：
        // en: The macros below are copied from PyTorch:
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
            // zh:
            // 作为最后一道防线，PyTorch在这里检查了ref_count_.load() >= INT_MAX。
            // 但我们认为这是不必要的，这应该在其他代码中得到保证。
            // en:
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
            // zh:
            // PyTorch在这里检查了weak_count_.load() == 1，
            // 因为它在~intrusive_ptr()中做了优化，导致可能出现弱引用为1的情况。
            // 但我们目前还没做优化，目前不需要判断这种情况。
            // en:
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
        // zh:
        // PyTorch在这里和两个fetch_sub中使用的内存顺序是std::memory_order_acq_rel，
        // 在weakcount的fetch_add中使用了std::memory_order_relaxed，
        // 但是我们使用了C++默认的std::memory_order_seq_cst。
        // 一般而言，如果不知道使用哪种内存顺序，那么使用seq_cst就不会出错。
        // 这是最安全的一种顺序，但可能带来性能损失（不过在X86平台上，性能损失可忽略）。
        // en:
        // PyTorch uses std::memory_order_acq_rel or std::memory_order_relaxed,
        // here we use std::memory_order_seq_cst following the default.
        // If you are not sure which memory order should be selected, then use
        // seq_cst, which is safe but might lead to performance penalties.
        // However, on most X86 platforms, the performance penalty is
        // negligible.

        // zh:
        // fetch_add()返回的是执行加法之前的值，所以需要额外的+1。
        // en:
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

  private:
    // zh:
    // 释放内存资源，但不析构自身。用于ref_count_ == 0但weak_count_ != 0的情况。
    // （如果weak_count_ == 0，那么可以直接析构自身）
    // en:
    // Release the memory resource, but do not destruct the object.
    // Be used when ref_count_ == 0, but weak_count_ != 0.
    // (When weak_count_ == 0, we can just destruct the object)
    virtual void release_storage() {}
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
