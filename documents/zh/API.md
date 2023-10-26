# API

### 其他

| API                         | 类型     | PyTorch对应 | 简述                                                      |
| --------------------------- | -------- | ----------- | --------------------------------------------------------- |
| tinytorch.hello_tinytorch() | function | --          | Hello World函数。分别使用python和C++输出Hello World信息。 |
|                             |          |             |                                                           |
|                             |          |             |                                                           |

# C/C++ API

### 张量库

| API                                                          | 类型                     | PyTorch对应                                                  | 简述                   |
| ------------------------------------------------------------ | ------------------------ | ------------------------------------------------------------ | ---------------------- |
| t_tensor                                                     | namespace                | c10, at                                                      | 张量库的命名空间       |
| t_tensor::intrusive_ptr_target                               | class                    | c10::intrusive_ptr_target                                    | 侵入式智能指针（对象） |
| t_tensor::intrusive_ptr_target:: {<br />ref_increase_atomic()<br />weak_increase_atomic()<br />ref_decrease_atomic()<br />weak_decrease_atomic() } | protected class function | c10::detail:: {<br />atomic_refcount_increment(refcount)<br />atomic_weakcount_increment(weakcount)<br />atomic_refcount_decrement(refcount)<br />atomic_weakcount_decrement(weakcount) } | 引用计数变更           |
| t_tensor::intrusive_ptr_target::<br />release_storage()      | private class function   | c10::intrusive_ptr_target<br />release_resources()           | 释放内存，但不析构     |
| t_tensor::intrusive_ptr\<TTarget\>                           | class                    | c10::intrusive_ptr\<TTarget, NullType\>                      | 侵入式智能指针         |
|                                                              |                          |                                                              |                        |
|                                                              |                          |                                                              |                        |

