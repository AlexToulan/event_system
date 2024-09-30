#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "GDelegate.hpp"
#include "VDelegate.hpp"

class EventListener {
public:
  EventListener() {}
  virtual ~EventListener() {}

  /// @brief Binds and event with a shared pointer so the caller can track listener lifecycle.
  /// @tparam TObj The subscriber type.
  /// @tparam ...VArgs Parameter pack for the method.
  /// @param func The method to call.
  /// @return Shared pointer to a Generic Delegate.
  template<class TObj, class... VArgs>
  static std::shared_ptr<GDelegate> bind(TObj* self, void(TObj::* func)(const VArgs&...)) {
    std::shared_ptr<GDelegate> delegate = std::make_shared<GDelegate>(GDelegate::create(self, func));
    self->addBinding(delegate);
    return delegate;
  }

  void pruneBindings() {
    auto eraseStartIt = std::remove_if(_delegates.begin(), _delegates.end(),
      [&](std::shared_ptr<GDelegate>& del) {
        // if the event system is the only owner of this delegate, it is a dangling reference
        return del.unique();
      } );
    _delegates.erase(eraseStartIt, _delegates.end());
  }

private:
  void addBinding(const std::shared_ptr<GDelegate>& delegate) {
    _delegates.push_back(delegate);
  }

  // Stored references for instance tracking
  std::vector<std::shared_ptr<GDelegate>> _delegates;
};
