#pragma once

#include <exception>
#include <string>

class bad_delegate_call : public std::exception {
public:
  bad_delegate_call(const char* msg)
    : _message(msg) {
  }
  const char* what() const throw()
  {
    return _message.c_str();
  }
private:
  std::string _message;
};
