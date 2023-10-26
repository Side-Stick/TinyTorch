#ifndef TINYTORCHCPPSRC_INTRUSIVE_PTR_HPP
#define TINYTORCHCPPSRC_INTRUSIVE_PTR_HPP

#include <atomic>
#include <memory>
#include <type_traits>

#include "utils/Exception.hpp"

namespace t_tensor {

class intrusive_ptr_target {
    template<typename T> friend class intrusive_ptr;
    template<typename T> friend class weak_intrusive_ptr;

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

    // zh: 拷贝构造函数
    // en: copy constructor
    intrusive_ptr_target(const intrusive_ptr_target& /*other*/) noexcept
            : intrusive_ptr_target() {}

    // zh: 移动赋值
    // en: move assignment
    intrusive_ptr_target& operator=(intrusive_ptr_target&& /*other*/) noexcept {
        return *this;
    }

    // zh: 拷贝赋值
    // en: copy assignment
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
            // However, we think the circumstance should be ensured in other
            // code, the condition should never be activated.
            throw TensorWarning(
                    /*warning_type_code=*/TensorWarning::REF_COUNT_WARN,
                    /*msg=*/"Trying to destruct an intrusive_ptr_target with "
                            "refcount_ = "
                            + std::to_string(ref_count_.load())
                            + ", (should be 0)",
                    /*traceback_source=*/"virtual ~intrusive_ptr_target()");
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
            throw TensorWarning(
                    /*warning_type_code=*/TensorWarning::REF_COUNT_WARN,
                    /*msg=*/"Trying to destruct an intrusive_ptr_target with "
                            "weak_count_ = "
                            + std::to_string(weak_count_.load())
                            + ", (should be 0)",
                    /*traceback_source=*/"virtual ~intrusive_ptr_target()");
        }
#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif
    }

    std::size_t ref_increase_atomic() noexcept {
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

    std::size_t weak_increase_atomic() noexcept {
        return weak_count_.fetch_add(1) + 1;
    }

    std::size_t ref_decrease_atomic() noexcept {
        return ref_count_.fetch_sub(1) - 1;
    }

    std::size_t weak_decrease_atomic() noexcept {
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
    static_assert(
            std::is_base_of<intrusive_ptr_target, TTarget>::value,
            "TypeError: the target of an intrusive_ptr must be "
            "inherited from intrusive_ptr_target.");

    template<typename T> friend class weak_intrusive_ptr;

    TTarget* target_;

  public:
    // zh: 从std::unique_ptr的移动构造函数
    // en: move constructor from std::unique_ptr
    explicit intrusive_ptr(std::unique_ptr<TTarget> source) noexcept
            : intrusive_ptr(source.release(), /*increase_refcount=*/true) {}

    // zh: 带隐式类型转换的移动构造函数
    // en: move constructor with implicit type convert
    template<typename From>
    intrusive_ptr(const intrusive_ptr<From>&& source) noexcept {
        static_assert(std::is_convertible<From, TTarget>::value,
                      "TypeError: trying to convert a wrong type in "
                      "implicit move constructor.");

        target_ = source.target_;
        source.target_ = nullptr;
    }

    // zh: 带隐式类型转换的浅拷贝构造函数（这里体现了“指针”的概念，可以复制多个ptr对象指向
    // 同一个target对象，并增加其引用计数）
    // en: Shallow copy constructor with implicit type convert (Here we
    // demonstrated the concept of "pointer", for multiple ptr objects points
    // to a same target object, and increases its ref_count_).
    template<typename From>
    intrusive_ptr(const intrusive_ptr<From>& source) {
        static_assert(std::is_convertible<From, TTarget>::value,
                      "TypeError: trying to convert a wrong type in "
                      "implicit copy constructor.");

        target_ = source.target_;
        try {
            retain_refcount_();
        } catch (TensorWarning &e) {
            e.add_track("template<typename From> intrusive_ptr("
                        "const intrusive_ptr<From>& source)");
            throw;
        }
    }

    // zh: 移动赋值
    // en: move assignment
    template<typename From>
    intrusive_ptr& operator=(const intrusive_ptr<From>&& source) & noexcept {
        // zh: 成员函数后缀的&和&&分别限制该方法只能被左值和右值对象调用。
        // en: The & and && after functions can limit it only be called by a
        // LValue and RValue object respectively.
        static_assert(std::is_convertible<From, TTarget>::value,
                      "TypeError: trying to convert a wrong type in "
                      "implicit move assignment.");

        target_ = std::move(source.target_);
        source.target_ = nullptr;
        return *this;
    }

    // zh: 拷贝赋值
    // en: copy assignment
    template<typename From>
    intrusive_ptr& operator=(const intrusive_ptr<From>& source) & noexcept {
        static_assert(std::is_convertible<From, TTarget>::value,
                      "TypeError: trying to convert a wrong type in "
                      "implicit copy assignment.");

        target_ = source.target_;
        target_->ref_increase_atomic();
        // TODO: find out why.
        // zh: PyTorch这里没有增加引用计数，不清楚为什么。
        // en: PyTorch DO NOT increase ref_count_ here, we don't know why.
        return *this;
    }

    ~intrusive_ptr() {
        clean_memory_();
    }

    TTarget* get() const noexcept {
        return target_;
    }

    TTarget& operator*() const noexcept {
        return *target_;
    }

    TTarget* operator->() const noexcept {
        // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDelete)
        return target_;
    }

    explicit operator bool() const noexcept {
        return target_ != nullptr;
    }

    bool defined() const noexcept {
        return target_ != nullptr;
    }

    size_t use_count() const noexcept {
        if (nullptr == target_) {
            return 0;
        }
        return target_->refcount_.load(std::memory_order_acquire);
    }

    size_t weak_use_count() const noexcept {
        if (nullptr == target_) {
            return 0;
        }
        return target_->weakcount_.load(std::memory_order_acquire);
    }

    bool unique() const noexcept {
        return use_count() == 1;
    }

    TTarget* release() noexcept {
        TTarget* result = target_;
        target_ = nullptr;
        return result;
    }

  private:
    // zh: 默认构造函数，不设置引用计数的情况可用于如detach等。
    // en: default constructor, the option that do not set ref counts, can be
    // used to tackle detach() and etc.
    explicit intrusive_ptr(TTarget* target, bool increase_refcount = true)
            : target_(target) {
        if (increase_refcount && (nullptr != target_)) {
            if (target_->ref_count_.load() != 0 || target_->weak_count_ != 0) {
                throw TensorWarning(
                        /*warning_type_code=*/TensorWarning::REF_COUNT_WARN,
                        /*msg=*/"The target of newly constructed intrusive_ptr"
                                "has non-zero reference count(s).",
                        /*traceback_source=*/
                            "explicit intrusive_ptr::intrusive_ptr("
                            "TTarget* target, bool increase_refcount = true)");
            }
            // zh: 智能指针是新创建的，不存在多线程竞争问题，无需考虑内存屏障顺序问题，
            // 使用relaxed顺序无风险，并能提高性能。
            // en: The intrusive_ptr is newly created, and other threads do not
            // have access to it. Thus, it's not necessary to care about memory
            // orders.
            target_->ref_count_.store(1, std::memory_order_relaxed);
            target_->weak_count_.store(1, std::memory_order_relaxed);
        }
    }

    void retain_refcount_() {
        if (nullptr != target_)
            return;

        if (0 == target_->ref_count_.load()) {
            throw TensorWarning(
                    /*warning_type_code=*/TensorWarning::REF_COUNT_WARN,
                    /*msg=*/"Trying to increase refcount after it reached "
                            "zero.",
                    /*traceback_source=*/
                        "void intrusive_ptr::retain_refcount_()");
        }
        target_->ref_increase_atomic();
    }

    void clean_memory_() {
        if (nullptr != target_ && 0 >= target_->ref_decrease_atomic()) {
            // zh: weak_count_一定大于等于ref_count_，所以只做一个判断提高性能。
            // en: weak_count_ is always >= ref_count_, so we can use only 1 if
            // to improve performance.
            bool should_delete = (0 >= target_->weak_count_
                    .load(std::memory_order_acquire));
            if (!should_delete) {
                const_cast<std::remove_const_t<TTarget>*>(target_)
                        ->release_storage();
            }
            // TODO: find out why.
            // zh:
            // PyTorch在这里对should_delete的判断标准是weak_count_ == 1，而且在执行
            // release_resources()【对应我们的release_storage()】之后又判断了一次
            // decrease之后的weak_count_ == 0。目前还没有搞懂为什么这么写，怀疑是与
            // ~intrusive_ptr_target()中提到的优化问题有关，参阅：
            // en:
            // PyTorch uses weak_count_ == 1 as the metric of should_delete,
            // and after release_resources (release_storage in our code), they
            // checked if decreased weak_count_ == 0. We don't know why to do
            // so yet, we inferred that might be related to the optimization
            // mentioned in ~intrusive_ptr_target(). See:

            // https://github.com/Side-Stick/TinyTorch/blob/1c7418727bbe539adec6e65d93cff493d6948bda/CppSrc/CppLibraries/Tensor/intrusive_ptr.hpp#L91
            if (should_delete) {
                delete target_;
                target_ = nullptr;
            }
        }
    }
};

template <class TTarget>
class weak_intrusive_ptr final {

};

} //namespace t_tensor

#endif //TINYTORCHCPPSRC_INTRUSIVE_PTR_HPP
