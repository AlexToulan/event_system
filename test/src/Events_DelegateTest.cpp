#include "gtest/gtest.h"
#include "ADelegate.hpp"
#include "VDelegate.hpp"

class MemberClass {
public:
  MemberClass() {
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
  MemberClass _class;
};

TEST_F(Events_DelegateTest, VDelegateExecute) {
  VDelegate<int, MemberClass> delGetData(&_class, &MemberClass::getData);

  // no parameter
  VDelegate<void, MemberClass> delSetOne(&_class, &MemberClass::setOne);
  delSetOne();
  EXPECT_EQ(1, delGetData());
  _class.setData(0);

  // multiple parameters
  VDelegate<void, MemberClass, int, int> delAdd(&_class, &MemberClass::setAdd);
  int num = 5;
  delAdd(num, num);
  EXPECT_EQ(num * 2, delGetData());
}

TEST_F(Events_DelegateTest, GDelegateTypeSafety) {
  VDelegate<int, MemberClass> a(&_class, &MemberClass::getData);
  GDelegate genDel = a.asGeneric();

  // Note: Macros don't like templates because the comma parses as another macro arg.
  // Adding extra '()' around our statement encapsulates it.
  EXPECT_NO_THROW((genDel.exec<int, MemberClass>()));
  EXPECT_THROW((genDel.exec<long int, MemberClass>()), std::bad_function_call);
  EXPECT_THROW((genDel.exec<int, MemberClass, int>(0)), std::bad_function_call);
}

TEST_F(Events_DelegateTest, VDelegateGenericConversion) {
  VDelegate<int, MemberClass> a(&_class, &MemberClass::getData);
  GDelegate genDel = a.asGeneric();

  _class.setData(1);
  int rtn = genDel.exec<int, MemberClass>();
  EXPECT_EQ(1, rtn);
}

TEST_F(Events_DelegateTest, ADelegateExecute) {
  ADelegate<MemberClass, int> delegate;
  delegate.add(&_class, &MemberClass::add);
  delegate(2);
  EXPECT_EQ(2, _class.getData());
}

TEST_F(Events_DelegateTest, ADelegateMultiExecuteInOrder) {
  ADelegate<MemberClass, int> delegate;
  // _data += 2;
  delegate.add(&_class, &MemberClass::add);
  // _data *= 2;
  delegate.add(&_class, &MemberClass::multiply);
  // _data = 0;
  // _data += 2; -> 2;
  // _data *= 2; -> 4;
  delegate(2);
  EXPECT_EQ(4, _class.getData());

  delegate.remove(&_class, &MemberClass::multiply);
  delegate(2);
  EXPECT_EQ(6, _class.getData());

  delegate.remove(&_class, &MemberClass::add);
  delegate(2);
  EXPECT_EQ(6, _class.getData());
}
