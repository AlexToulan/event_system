# Overview
A code experiment demonstrating a thread-safe C++20 event system intended for prototyping purposed.
## Event System
A light-weight event system designed for prototyping purposes. `EventSystem` and `EventListener` provide a basic framework for managing events with no user-defined event data-objects required for passing data to listeners. Users can bind `EventListener` derived class methods to an `EventSystem` and execute them with `EventSystem::publish`. Note that event handling methods can have a return type, but the returning object can not be accessed by the `EventSystem`. Additionally, event handling methods must have const reference type parameters, this gives listeners full control over data copying.

The main drawback of this implementation is that type-safety for publishing events is checked at run-time using `std::type_info::hash_code()`, which does not guarantee uniqueness (this may be resolved in future development using `std::type_index`). This run-time type-safety limitation is by design for flexibility and ease of use for prototyping or none-critical quality of life features. `EventSystem` is able to take a listener reference and function signature, convert it to a Generic Delegate (`GDelegate`), store it in the event binding map and call it using the parameter-pack provided by `EventSystem::publish`.

For production use cases where type-safety is critical, users are advised to only use Variadic Delegates `VDelegate` and Aggregate Delegates `ADelegate`.
### Examples
#### Basic Usage
```
#include "EventSystem.hpp"
#include "EventListener.hpp"

class MyEventHandler : public EventListener {
public:
  void onEvent(const std::string& message) {
    std::cout << "Received event with message: " << message << std::endl;
  }
};

void main() {
  EventSystem<std::string> eventSystem("EventSystemName");
  MyEventHandler handler;

  // Bind the event to the handler method
  eventSystem.bind("event1", &handler, &MyEventHandler::onEvent);

  // Publish event
  eventSystem.publish("event1", "Hello, Event!");

  // Unsubscribe from the event
  eventSystem.unsubscribe("event1", &handler);

  // Dispatch an event (should do nothing if unsubscribed)
  eventSystem.publish("event1", "Hello, Event!");
}
```
#### Advanced Usage
##### MyEvents.hpp
```
#include "EventSystem.hpp"

#include <cstddef>  // size_t
#include <memory>   // std::unique_ptr

enum EEventType : size_t {
  NA = 0,
  FIRST_EVENT,
  SECOND_EVENT
};

namespace Events {
  extern std::unique_ptr<EventSystem<EEventType>> MyEvents;
}
```
##### AdvancedExample.cpp
```
// Include this wherever Events::MyEvents needs to be used
#include "MyEvents.hpp"

class MyEventHandler : public EventListener {
public:
  void subscribe() {
    Events::MyEvents.bind(EEventType::FIRST_EVENT, this, &MyEventHandler::firstEvent);
    Events::MyEvents.bind(EEventType::SECOND_EVENT, this, &MyEventHandler::secondEvent);
  }

  void firstEvent(const int& value) {
    std::cout << "Received event with value: " << value << std::endl;
  }

  void secondEvent(const int& a, const int& b) {
    std::cout << "Received event with values: " << a << " and " << b << std::endl;
  }
};

// Declare this once
std::unique_ptr<EventSystem<EEventType>> Events::MyEvents;

void main() {
  // Initialize event system
  Events::MyEvents = std::make_unique<EventSystem<EEventType>>();
  // Initialize listener
  MyEventHandler handler;
  handler.subscribe();

  int value = 0;
  int a = 1;
  int b = 2;

  Events::MyEvents.publish(EEventType::FIRST_EVENT, value);
  Events::MyEvents.publish(EEventType::SECOND_EVENT, a, b);
}
```
## Variadic Delegate (VDelegate.hpp)
A versatile delegate that allows for type-safe callbacks with minimal dependencies. It is designed to work with member functions of objects, making it particularly useful for event handling and callback mechanisms in various applications.
### Example
Let's assume we have a simple object with a method that we want to delegate.
```
#include "VDelegate.hpp"
#include <iostream>

class Calculator {
public:
  int add(const int& a, const int& b) {
      return a + b;
  }
};

void main() {
  Calculator calculator;
  VDelegate<int, Calculator, int, int> addDel(&calculator, &Calculator::add);
  int a = 3;
  int b = 5;
  int sum = addDel(a, b);
  std::cout << a << " + " << b " = " << sum << std::endl; 
  return 0;
}
```
## Aggregate Delegate (GDelegate.hpp)
Extends the functionality provided by `VDelegate.hpp` to manage a collection (or aggregation) of delegates. It allows for handling multiple delegates in a sequential manner, which is particularly useful when you need to call several methods in response to an event or command.
### Example
```
#include "VDelegate.hpp"
#include "ADelegate.hpp"
#include <iostream>

class MyObject {
public:
  void myMethod(const std::string& message) {
      std::cout << "Received message in myMethod: " << message << std::endl;
  }
};

void main() {
  MyObject obj1, obj2;

  // Create delegates for each object's method
  VDelegate<void, MyObject, std::string> delegate1(&obj1, &MyObject::myMethod);
  VDelegate<void, MyObject, std::string> delegate2(&obj2, &MyObject::myMethod);

  // Create an ADelegate to manage the delegates
  ADelegate<MyObject, std::string> ad;
  ad += delegate1;
  ad += delegate2;

  // Dispatch a string event to all delegates
  ad("Hello, Multiple Delegates!");

  // Remove one of the delegates and dispatch again
  ad -= delegate1;
  ad("Only remaining delegate should receive this message now.");
}
```
# Conclusion
While this event system provides a simple and flexible way to manage events for prototyping purposes, it should not be used in production environments where type-safety is critical. The use of `std::type_info::hash_code()` for type identification can lead to collisions, which may result in unexpected behavior. For robust and type-safe event management, consider using other delegate types that enforce compile-time type checking.
