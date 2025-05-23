#ifndef __MDVECTOR_TENSOR_EXPR_H__
#define __MDVECTOR_TENSOR_EXPR_H__

#include "../simd/simd.h"

// ======================== 表达式模板基类 ========================
template <class Derived, class Policy>
class TensorExpr {
 public:
  const Derived& derived() const { return static_cast<const Derived&>(*this); }

  size_t size() const { return derived().size(); }

  auto extents() const { return derived().extents(); }

  // 左值
  auto eval_simd(size_t i) const { return static_cast<const Derived&>(*this).eval_simd(i); }

  template <typename Dest>
  void eval_to(Dest* dest) const {
    const size_t n = size();
    constexpr size_t pack_size = simd<Dest>::pack_size;
    size_t i = 0;

    for (; i + pack_size <= n; i += pack_size) {
      auto simd_val = derived().template eval_simd<std::remove_const_t<Dest>>(i);
      Policy::template store<std::remove_const_t<Dest>>(dest + i, simd_val);
    }

    // 使用掩码处理尾部元素
    const size_t remaining = n - i;
    auto simd_val = derived().template eval_simd_mask<std::remove_const_t<Dest>>(i);
    Policy::template mask_store<std::remove_const_t<Dest>>(dest + i, remaining, simd_val);
  }
};

#endif  // __TENSOR_EXPR_H__