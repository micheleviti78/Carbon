/**
 ******************************************************************************
 * @file           function_ref.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Mar. 2023
 * @brief          Non-owning reference to a callable object
 ******************************************************************************
 * @attention
 * Copyright (c) 2022 Michele Viti.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

//===- STLForwardCompat.h - Library features from future STLs ------C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains library features backported from future STL versions.
///
/// These should be replaced with their STL counterparts as the C++ version LLVM
/// is compiled with is updated.
///
//===----------------------------------------------------------------------===//

//===- llvm/ADT/STLFunctionalExtras.h - Extras for <functional> -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains some extension to <functional>.
//
// No library is required when using these functions.
//
//===----------------------------------------------------------------------===//

#pragma once

#include <optional>
#include <type_traits>

#include <cstdint>
#include <type_traits>
#include <utility>

namespace CARBON {

//===----------------------------------------------------------------------===//
//     Features from C++20
//===----------------------------------------------------------------------===//

template <typename T>
struct remove_cvref // NOLINT(readability-identifier-naming)
{
  using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <typename T>
using remove_cvref_t // NOLINT(readability-identifier-naming)
    = typename llvm::remove_cvref<T>::type;

//===----------------------------------------------------------------------===//
//     Features from C++23
//===----------------------------------------------------------------------===//

// TODO: Remove this in favor of std::optional<T>::transform once we switch to
// C++23.
template <typename T, typename Function>
auto transformOptional(const std::optional<T> &O, const Function &F)
    -> std::optional<decltype(F(*O))> {
  if (O)
    return F(*O);
  return std::nullopt;
}

// TODO: Remove this in favor of std::optional<T>::transform once we switch to
// C++23.
template <typename T, typename Function>
auto transformOptional(std::optional<T> &&O, const Function &F)
    -> std::optional<decltype(F(*std::move(O)))> {
  if (O)
    return F(*std::move(O));
  return std::nullopt;
}

//===----------------------------------------------------------------------===//
//     Extra additions to <functional>
//===----------------------------------------------------------------------===//

/// An efficient, type-erasing, non-owning reference to a callable. This is
/// intended for use as the type of a function parameter that is not used
/// after the function in question returns.
///
/// This class does not own the callable, so it is not in general safe to store
/// a function_ref.
template<typename Fn> class function_ref;

template<typename Ret, typename ...Params>
class function_ref<Ret(Params...)> {
  Ret (*callback)(intptr_t callable, Params ...params) = nullptr;
  intptr_t callable;

  template<typename Callable>
  static Ret callback_fn(intptr_t callable, Params ...params) {
    return (*reinterpret_cast<Callable*>(callable))(
        std::forward<Params>(params)...);
  }

public:
  function_ref() = default;
  function_ref(std::nullptr_t) {}

  template <typename Callable>
  function_ref(
      Callable &&callable,
      // This is not the copy-constructor.
      std::enable_if_t<!std::is_same<remove_cvref_t<Callable>,
                                     function_ref>::value> * = nullptr,
      // Functor must be callable and return a suitable type.
      std::enable_if_t<std::is_void<Ret>::value ||
                       std::is_convertible<decltype(std::declval<Callable>()(
                                               std::declval<Params>()...)),
                                           Ret>::value> * = nullptr)
      : callback(callback_fn<std::remove_reference_t<Callable>>),
        callable(reinterpret_cast<intptr_t>(&callable)) {}

  Ret operator()(Params ...params) const {
    return callback(callable, std::forward<Params>(params)...);
  }

  explicit operator bool() const { return callback; }
};

} // namespace CARBON
