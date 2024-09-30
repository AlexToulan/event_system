#pragma once

#include "GDelegate.hpp"

/// @brief Variadic member function Delegate.
/// @tparam TReturn Delegate return type.
/// @tparam TObj Object pointer type.
/// @tparam ...VArgs Parameter pack.
template<class TReturn, class TObj, class... VArgs>
class VDelegate {
public:
  VDelegate(const TObj* objPtr, TReturn(TObj::* func)(const VArgs&...))
    : _objPtr(const_cast<TObj*>(objPtr))
    , _func(func) {
  }

  VDelegate(const VDelegate& other)
    : _objPtr(other._objPtr)
    , _func(other._func) {
  }

  VDelegate(const VDelegate&& other) {
    _objPtr = other._objPtr;
    _func = other._func;

    other._objPtr = nullptr;
    other._func = nullptr;
  }

  VDelegate& operator=(const VDelegate& other) {
    _objPtr = other._objPtr;
    _func = other._func;
    return *this;
  }

  /// @brief Executes the Delegate.
  /// @param ...vargs Arguments passed to the method pointer.
  /// @return Return value of the method pointer.
  TReturn exec(const VArgs&... vargs) {
    return (_objPtr->*_func)(vargs...);
  }

  /// @brief Executes the Delegate.
  /// @param ...vargs Arguments passed to the method pointer.
  /// @return Return value of the method pointer.
  TReturn operator()(const VArgs&... vargs) {
    return (_objPtr->*_func)(vargs...);
  }

  /// @brief Equals operator.
  /// @param other Other instance to compare to.
  /// @return True if both objPtr and func are the same, false otherwise.
  bool operator==(const VDelegate<TReturn, TObj, VArgs...>& other) const {
    return _objPtr == other._objPtr &&  _func == other._func;
  }

  /// @brief Not-equals operator.
  /// @param other Other instance to compare to.
  /// @return True if objPtr or func are NOT the same, false otherwise.
  bool operator!=(const VDelegate<TReturn, TObj, VArgs...>& other) const {
    return _objPtr != other._objPtr || _func != other._func;
  }

  /// @brief Checks if this Delegate calls the incoming instance.
  /// @param caller Instance to check ownership against.
  /// @return True if the Delegate references the provided caller.
  bool isCaller(const TObj* caller) const {
    return _objPtr == caller;
  }

  /// @brief Creates a generic copy of this Delegate.
  /// @return The generic copy.
  GDelegate asGeneric() {
    return GDelegate::create(_objPtr, _func);
  }

private:
  TObj* _objPtr;
  TReturn(TObj::* _func)(const VArgs&...);
};
