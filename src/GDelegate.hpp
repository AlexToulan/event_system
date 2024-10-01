#pragma once

#include "Exceptions.hpp"

#include <typeinfo>

/// @brief Generic member function Delegate for storage and explicit variadic execution.
class GDelegate {
public:
  /// @brief Default constructor.
  GDelegate()
    : _objPtr(nullptr)
    , _func(nullptr)
    , _typeHashCode(0) {
  }

  GDelegate(const GDelegate& other)
    : _objPtr(other._objPtr)
    , _func(other._func)
    , _typeHashCode(other._typeHashCode) {
  }

  GDelegate& operator=(const GDelegate& other) {
    _objPtr = other._objPtr;
    _func = other._func;
    _typeHashCode = other._typeHashCode;
    return *this;
  }

  template<class TReturn, class TObj, class... VArgs>
  static GDelegate create(TObj* objPtr, TReturn(TObj::* func)(const VArgs&...)) {
    return GDelegate(static_cast<void*>(objPtr), reinterpret_cast<void(Object::*)()>(func), typeid(func).hash_code());
  }

  /// @brief Explicit variadic execution of a generic VDelegate.
  /// @tparam TReturn Delegate return type.
  /// @tparam TObj Object pointer type.
  /// @tparam ...VArgs Parameter pack.
  /// @param ...vargs Parameter pack arguments.
  /// @return Return value returned by the underlying VDelegate.
  template<class TReturn, class TObj, class... VArgs>
  TReturn exec(const VArgs&... vargs) {
    auto objPtr = static_cast<TObj*>(_objPtr);
    TReturn(TObj::* func)(const VArgs&...) = reinterpret_cast<TReturn(TObj::*)(const VArgs&...)>(_func);
    // Ensure the types of vargs match those expected by the underlying VDelegate
    if (_typeHashCode != typeid(func).hash_code()) {
      throw bad_delegate_call("GDelegate: Run-time type-safety check failed. Ensure execution function signature matches underlying member function signature.");
    }
    return (objPtr->*func)(vargs...);
  }

  /// @brief Equals operator.
  /// @param other Other instance to compare to.
  /// @return True if both objPtr and func are the same, false otherwise.
  bool operator==(const GDelegate& other) const {
    return _objPtr == other._objPtr &&  _func == other._func;
  }

  /// @brief Not-equals operator.
  /// @param other Other instance to compare to.
  /// @return True if objPtr or func are NOT the same, false otherwise.
  bool operator!=(const GDelegate& other) const {
    return _objPtr != other._objPtr || _func != other._func;
  }

  /// @brief Checks if this Delegate calls the incoming instance.
  /// @param caller Instance to check ownership against.
  /// @return True if the Delegate references the provided caller.
  template<class TObj>
  bool isCaller(TObj* caller) const {
    return _objPtr == static_cast<void*>(caller);
  }

private:
  /// @brief Generic placeholder object type
  class Object {};

  /// @brief Private default constructor.
  /// @param objPtr Generic object reference storage.
  /// @param func Generic method storage. 
  GDelegate(void* objPtr, void(Object::* func)(), size_t typeHashCode)
    : _objPtr(objPtr)
    , _func(func)
    , _typeHashCode(typeHashCode) {
  }

  void* _objPtr;
  void(Object::* _func)();
  size_t _typeHashCode;
};
