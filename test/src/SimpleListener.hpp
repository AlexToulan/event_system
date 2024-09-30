#pragma once

#include "EventListener.hpp"

class SimpleListener : public EventListener {
public:
  SimpleListener() : EventListener(), _number(0) {}
  virtual ~SimpleListener(){}

  void receivedNumbers(const int& num) {
    // _number += unpack<int>(event);
    _number += num;
  }

  const int getNumber() const {
    return _number;
  }

private:
  int _number;
};
