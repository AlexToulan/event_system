#pragma once

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <type_traits>
#include <vector>
#include <Log.hpp>

#include "GDelegate.hpp"
#include "EventListener.hpp"
// #include "Log.hpp"

/// @brief This class allows for the registration and triggering of events with delegates,
/// functionality encapsulated in GDelegate and listeners managed by EventListener, bound to
/// unique user defined keys.
/// Execution: EventListener Subscribe (key, method) -> EventSystem Publish (key, params) -> GDelegate -> EventListener Method Call (params)

template<class TKey>
class EventSystem final {
public:
  EventSystem(std::string name = "EventSystem") {
    _name = name;
  }

  ~EventSystem() {
  }

  /// @brief Publishes an event on the calling thread.
  /// @param key The event key to publish under.
  /// @param data The data to pass to the listener.
  /// @return True: if there are no stale event bindings.
  template<typename ...VArgs>
  bool publish(const TKey& key, const VArgs&... vargs) const {
    bool succ = true;
    auto bindingIt = _eventBindings.find(key);
    if (bindingIt != _eventBindings.end()) {
      for (auto& del : _eventBindings.at(key)) {
        if (del.unique()) {
          Log().error("EventListener destroyed without unsubscribing from event system bindings.");
          succ = false;
        } else {
          try {
            del->template exec<void, EventListener, VArgs...>(vargs...);
          } catch (const bad_delegate_call& e) {
            Log().error("Exception caught in \"{}\": {}", _name, e.what());
            succ = false;
          }
        }
      }  
    }
    return succ;
  }

  /// @brief Subscribe to a single event.
  /// @param key The event key to listen for.
  /// @param delegate_ptr Pointer to shared generic delegate.
  bool subscribe(const TKey& key, const std::shared_ptr<GDelegate>& delegate_ptr) {
    std::unique_lock<decltype(_bindingMutex)> lock(_bindingMutex);
    // enforce unique keys
    if (_eventBindings.find(key) == _eventBindings.end()) {
      _eventBindings[key] = std::vector<std::shared_ptr<GDelegate>>();
    }
    // prevent duplicate delegates under the same event key
    auto it = std::find_if(_eventBindings[key].begin(), _eventBindings[key].end(),
    [&](const std::shared_ptr<GDelegate> del_ptr) {
      return (*del_ptr) == (*delegate_ptr);
    });
    if (it == _eventBindings[key].end()) {
      _eventBindings[key].emplace_back(delegate_ptr);
    } else {
      // users can still subscribe with duplicate delegate instances if under different event types
      Log().error("key: {} attempted to subscribe more than once with the same delegate instance", key);
      return false;
    } return true;
  }

  /// @brief Bind delegate and subscribe to a single event.
  /// @param key The event key to listen for.
  /// @param self The instance object to call.
  /// @param func The method to call.
  /// @tparam TObj The subscriber type.
  /// @tparam ...VArgs Parameter pack for the method.
  /// @return Shared pointer to a Generic Delegate.
  template<class TObj, class... VArgs>
  bool bind(const TKey& key, TObj* self, void(TObj::* func)(const VArgs&...)) {
    return subscribe(key, EventListener::bind(self, func));
  }

  /// @brief Unsubscribe from all events for this EventListener
  /// @param listener The EventListener that's unsubscribing
  void unsubscribe(EventListener* listener) {
    std::unique_lock<decltype(_bindingMutex)> lock(_bindingMutex);
    for (auto& [key, delegates] : _eventBindings) {
      auto eraseStartIt = std::remove_if(delegates.begin(), delegates.end(),
        [&](std::shared_ptr<GDelegate>& del) {
          return del->isCaller(listener);
        } );
      delegates.erase(eraseStartIt, delegates.end());
    }
    listener->pruneBindings();
  }

  /// @brief Unsubscribe from a single event type for this EventListener
  /// @param key Key to unsubscribe from
  /// @param listener EventListener that's unsubscribing
  void unsubscribe(const TKey& key, EventListener* listener) {
    auto bindingIt = _eventBindings.find(key);
    if (bindingIt != _eventBindings.end()) {
      std::unique_lock<decltype(_bindingMutex)> lock(_bindingMutex);
      auto eraseStartIt = std::remove_if(bindingIt->second.begin(), bindingIt->second.end(),
        [&](std::shared_ptr<GDelegate>& del) {
          return del->isCaller(listener);
        } );
      _eventBindings[key].erase(eraseStartIt, _eventBindings[key].end());
    } listener->pruneBindings();
  }

  /// @brief removes delegates with dangling references
  void pruneBindings() {
    std::unique_lock<decltype(_bindingMutex)> lock(_bindingMutex);
    for (auto& [key, delegates] : _eventBindings) {
      auto eraseStartIt = std::remove_if(delegates.begin(), delegates.end(),
        [&](std::shared_ptr<GDelegate>& del) {
          // if the event system is the only owner of this delegate, it is a dangling reference
          return del.unique();
        } );
      delegates.erase(eraseStartIt, delegates.end());
    }
  }

private:
  std::string _name;
  std::mutex _bindingMutex;
  std::map<TKey, std::vector<std::shared_ptr<GDelegate>>> _eventBindings;
};
