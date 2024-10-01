#include "gtest/gtest.h"
#include "ADelegate.hpp"
#include "VDelegate.hpp"
#include "TDelegate.hpp"

class ExampleClass {
public:
  ExampleClass() {
    _data = 0;
  }
  void setData(const int& data) {
    _data = data;
  }
  void setOne() {
    _data = 1;
  }
  void setAdd(const int& a, const int& b) {
    _data = a + b;
  }
  void add(const int& a) {
    _data += a;
  }
  void multiply(const int& a) {
    _data *= a;
  }
  int getData() {
    return _data;
  }

private:
  int _data;
};

/// @brief Tests for GDelegate, VDelegate and ADelegate
class Events_DelegateTest : public testing::Test {
protected:
  void SetUp() override {
  };

  void TearDown() override {
  };
  ExampleClass _class;
};

TEST_F(Events_DelegateTest, VDelegateExecute) {
  VDelegate<int, ExampleClass> delGetData(&_class, &ExampleClass::getData);

  // no parameter
  VDelegate<void, ExampleClass> delSetOne(&_class, &ExampleClass::setOne);
  delSetOne();
  EXPECT_EQ(1, delGetData());
  _class.setData(0);

  // multiple parameters
  VDelegate<void, ExampleClass, int, int> delAdd(&_class, &ExampleClass::setAdd);
  int num = 5;
  delAdd(num, num);
  EXPECT_EQ(num * 2, delGetData());
}

TEST_F(Events_DelegateTest, VDelegateGenericConversion) {
  VDelegate<int, ExampleClass> a(&_class, &ExampleClass::getData);
  GDelegate genDel = a.asGeneric();

  _class.setData(1);
  int rtn = genDel.exec<int, ExampleClass>();
  EXPECT_EQ(1, rtn);
}

TEST_F(Events_DelegateTest, ADelegateExecute) {
  ADelegate<ExampleClass, int> delegate;
  delegate.add(&_class, &ExampleClass::add);
  delegate(2);
  EXPECT_EQ(2, _class.getData());
}

TEST_F(Events_DelegateTest, ADelegateMultiExecuteInOrder) {
  ADelegate<ExampleClass, int> delegate;
  // _data += 2;
  delegate.add(&_class, &ExampleClass::add);
  // _data *= 2;
  delegate.add(&_class, &ExampleClass::multiply);
  // _data = 0;
  // _data += 2; -> 2;
  // _data *= 2; -> 4;
  delegate(2);
  EXPECT_EQ(4, _class.getData());

  delegate.remove(&_class, &ExampleClass::multiply);
  delegate(2);
  EXPECT_EQ(6, _class.getData());

  delegate.remove(&_class, &ExampleClass::add);
  delegate(2);
  EXPECT_EQ(6, _class.getData());
}

TEST_F(Events_DelegateTest, GDelegateTypeSafety) {
  VDelegate<int, ExampleClass> a(&_class, &ExampleClass::getData);
  GDelegate genDel = a.asGeneric();

  // Note: Macros don't like templates because the comma parses as another macro arg.
  // Adding extra '()' around our statement encapsulates it.
  EXPECT_NO_THROW((genDel.exec<int, ExampleClass>()));
  EXPECT_THROW((genDel.exec<long int, ExampleClass>()), bad_delegate_call);
  EXPECT_THROW((genDel.exec<int, ExampleClass, int>(0)), bad_delegate_call);
}

TEST_F(Events_DelegateTest, TDelegatePrimeAndExecute) {
  // Test priming the delegate and then executing it
  TDelegate<void, ExampleClass, int, int> threadDelegate(&_class, &ExampleClass::setAdd);
  threadDelegate.prime(5, 3); // prime with arguments 5 and 3
  EXPECT_NO_THROW(threadDelegate()); // execute the delegate
  EXPECT_EQ(_class.getData(), 8); // check if data is set correctly
}

TEST_F(Events_DelegateTest, TDelegateExecuteWithoutPrime) {
  // Test executing a primed delegate without priming it first
  TDelegate<void, ExampleClass, int, int> threadDelegate(&_class, &ExampleClass::setAdd);
  EXPECT_THROW(threadDelegate(), bad_delegate_call); // should throw because not primed
}

TEST_F(Events_DelegateTest, TDelegateEquality) {
  // Test equality operator for delegates

  TDelegate<void, ExampleClass, int> delegate1(&_class, &ExampleClass::add);
  TDelegate<void, ExampleClass, int> delegate2(&_class, &ExampleClass::add);
  TDelegate<void, ExampleClass, int> delegate3(&_class, &ExampleClass::multiply);
  EXPECT_TRUE(delegate1 == delegate2); // delegates should be equal
  EXPECT_TRUE(delegate1 != delegate3); // delegates should not be equal

  EXPECT_FALSE(delegate1 != delegate2); // delegates should be equal
  EXPECT_FALSE(delegate1 == delegate3); // delegates should not be equal
}
