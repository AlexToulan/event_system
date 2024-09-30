#pragma once

#include <algorithm>
#include <vector>

#include "VDelegate.hpp"
/// @brief A collection of VDelegates that can be called sequentially. VDelegates must return void.
/// @tparam TObj Member object type.
/// @tparam ...VArgs Parameter pack for the underlying delegate parameters.
template<class TObj, class... VArgs>
class ADelegate {
public:
  /// @brief Default constructor.
  ADelegate() {
  }

  /// @brief Constructs with a single delegate (use VDelegate if only one delegate needed).
  /// @param objectPtr A pointer to the listening object.
  /// @param func Member method to be called.
  ADelegate(const TObj* objectPtr, void(TObj::* func)(const VArgs&...)) {
    add(VDelegate<void, TObj, VArgs...>(objectPtr, func));
  }

  /// @brief Copy constructor.
  /// @param other Object to read from.
  ADelegate(const ADelegate& other) {
    _delegates = other._delegates;
  }

  /// @brief Move constructor
  /// @param other Object to move.
  ADelegate(const ADelegate&& other) {
    _delegates = other._delegates;
    other._delegates.clear();
  }

  /// @brief Subscribe from another ADelegate's entire delegate list.
  /// @param right Other ADelegate object.
  /// @return This object instance.
  ADelegate& operator+=(const ADelegate& right) {
    for (auto& del : right._delegates) {
      add(del);
    } return *this;
  }

  /// @brief Uniquely subscribe to a VDelegate.
  /// @param right Subscription.
  /// @return This object instance.
  ADelegate& operator+=(const VDelegate<void, TObj, VArgs...>& right) {
    add(right);
    return *this;
  }

  /// @brief Unsubscribes from a VDelegate.
  /// @param right Unsubscription.
  /// @return This object instance.
  ADelegate& operator-=(const VDelegate<void, TObj, VArgs...>& right) {
    remove(right);
    return *this;
  }

  /// @brief Execute all VDelegates.
  /// @param ...vargs Parameter pack for the underlying delegates.
  void exec(const VArgs&... vargs) {
    for (auto& del : _delegates) {
      del(vargs...);
    }
  }

  /// @brief Execute all VDelegates.
  /// @param ...vargs Parameter pack for the underlying delegates.
  void operator()(const VArgs&... vargs) {
    exec(vargs...);
  }

  /// @brief Uniquely subscribe to a VDelegate.
  /// @param right Subscription delegate.
  void add(const VDelegate<void, TObj, VArgs...>& right) {
    if (std::find(_delegates.begin(), _delegates.end(), right) == _delegates.end()) {
      _delegates.push_back(right);
    }
  }

  /// @brief Uniquely subscribe to a VDelegate.
  /// @param objectPtr A pointer to the listening object.
  /// @param func Member method to be called.
  void add(const TObj* objectPtr, void(TObj::* func)(const VArgs&...)) {
    add(VDelegate(objectPtr, func));
  }

  /// @brief Remove a VDelegate subscription.
  /// @param right Delegate reference to remove.
  void remove(const VDelegate<void, TObj, VArgs...>& right) {
    auto end = std::remove(_delegates.begin(), _delegates.end(), right);
    _delegates.erase(end, _delegates.end());
  }

  void remove(const TObj* objectPtr, void(TObj::* func)(const VArgs&...)) {
    remove(VDelegate(objectPtr, func));
  }

private:
  std::vector<VDelegate<void, TObj, VArgs...>> _delegates;
};
