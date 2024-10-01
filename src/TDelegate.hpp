#pragma once

#include "Exceptions.hpp"
#include "GDelegate.hpp"

/// @brief Variadic member function Delegate that can store arguments and execute at a later time on another thread.
/// @tparam TReturn Delegate return type.
/// @tparam TObj Object pointer type.
/// @tparam ...VArgs Parameter pack.
template<class TReturn, class TObj, class... VArgs>
class TDelegate {
public:
  TDelegate(const TObj* objPtr, TReturn(TObj::* func)(const VArgs&...))
    : _objPtr(const_cast<TObj*>(objPtr))
    , _func(func)
    , _primed(false)
    , _params() {
  }

  TDelegate(const TDelegate& other)
    : _objPtr(other._objPtr)
    , _func(other._func)
    , _primed(other._primed)
    , _params(other._params) {
  }

  TDelegate(const TDelegate&& other) {
    _objPtr = other._objPtr;
    _func = other._func;
    _primed = other._primed;
    _params = other._params;

    other._objPtr = nullptr;
    other._func = nullptr;
    other._primed = false;
    other._params = std::tuple<VArgs...>();
  }

  TDelegate& operator=(const TDelegate& other) {
    _objPtr = other._objPtr;
    _func = other._func;
    _primed = other._primed;
    return *this;
  }

  /// @brief Checks if the delegate is primed for execution.
  /// @return True if delegate is primed, otherwise False.
  bool isPrimed() const {
    return _primed;
  }

  /// @brief Primes the delegate storing arguments for later execution.
  /// @param ...vargs Arguments passed to the method pointer.
  void prime(const VArgs&... vargs) {
    std::unique_lock<decltype(_execMutex)> lock(_execMutex);
    _primed = true;
    _params = std::make_tuple(vargs...);
  }

  /// @brief Executes the Delegate.
  /// @param ...vargs Arguments passed to the method pointer.
  /// @return Return value of the method pointer.
  TReturn exec() {
    if (!_primed)
      throw bad_delegate_call("Execution attempted on a TDelegate that was not primed.");
    std::unique_lock<decltype(_execMutex)> lock(_execMutex);
    _primed = false;
    return std::apply(std::bind_front(_func, _objPtr), _params);
  }

  /// @brief Executes the Delegate.
  /// @return Return value of the method pointer.
  TReturn operator()() {
    return exec();
  }

  /// @brief Equals operator.
  /// @param other Other instance to compare to.
  /// @return True if both objPtr and func are the same, false otherwise.
  bool operator==(const TDelegate<TReturn, TObj, VArgs...>& other) const {
    return _objPtr == other._objPtr &&  _func == other._func;
  }

  /// @brief Not-equals operator.
  /// @param other Other instance to compare to.
  /// @return True if objPtr or func are NOT the same, false otherwise.
  bool operator!=(const TDelegate<TReturn, TObj, VArgs...>& other) const {
    return _objPtr != other._objPtr || _func != other._func;
  }

  /// @brief Checks if this Delegate calls the incoming instance.
  /// @param caller Instance to check ownership against.
  /// @return True if the Delegate references the provided caller.
  bool isCaller(const TObj* caller) const {
    return _objPtr == caller;
  }

private:
  TObj* _objPtr;
  TReturn(TObj::* _func)(const VArgs&...);
  std::atomic_bool _primed;
  std::tuple<VArgs...> _params;
  std::mutex _execMutex;
};
