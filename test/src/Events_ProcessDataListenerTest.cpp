#include <thread>

#include "gtest/gtest.h"
#include "TestType.hpp"
#include "EventSystem.hpp"

#include "ProcessDataListener.hpp"

/// @brief Tests for sending and receiving data
class Events_ProcessDataListenerTest : public testing::Test {
protected:
  void SetUp() override {
    numbers_1 = { 5, 4, 3, 2, 1 };
    numbers_2 = { 1, 2, 3, 4, 5 };
  };

  void TearDown() override {
  };
  ProcessDataListener a;
  ProcessDataListener b;
  std::vector<int> numbers_1;
  std::vector<int> numbers_2;
  EventSystem<ETestType> events;
};

TEST_F(Events_ProcessDataListenerTest, MultipleSubscribe) {
  EXPECT_TRUE(events.bind(ETestType::VECTOR_INT, &a, &ProcessDataListener::receivedNumbers));
  EXPECT_TRUE(events.bind(ETestType::VECTOR_INT, &b, &ProcessDataListener::receivedNumbers));

  EXPECT_TRUE(events.publish(ETestType::VECTOR_INT, numbers_1));
  EXPECT_EQ(numbers_1, a.getNumbers());
  EXPECT_EQ(numbers_1, b.getNumbers());

  EXPECT_TRUE(events.publish(ETestType::VECTOR_INT, numbers_2));
  EXPECT_EQ(numbers_2, a.getNumbers());
  EXPECT_EQ(numbers_2, b.getNumbers());
}

TEST_F(Events_ProcessDataListenerTest, Unsubscribe) {
  EXPECT_TRUE(events.bind(ETestType::VECTOR_INT, &a, &ProcessDataListener::receivedNumbers));
  EXPECT_TRUE(events.bind(ETestType::VECTOR_INT, &b, &ProcessDataListener::receivedNumbers));

  EXPECT_TRUE(events.publish(ETestType::VECTOR_INT, numbers_1));
  EXPECT_EQ(numbers_1, a.getNumbers());
  EXPECT_EQ(numbers_1, b.getNumbers());

  events.unsubscribe(ETestType::VECTOR_INT, &b);
  EXPECT_TRUE(events.publish(ETestType::VECTOR_INT, numbers_2));
  EXPECT_EQ(numbers_2, a.getNumbers());
  EXPECT_EQ(numbers_1, b.getNumbers());

  events.unsubscribe(ETestType::VECTOR_INT, &a);
  EXPECT_TRUE(events.publish(ETestType::VECTOR_INT, numbers_1));
  EXPECT_EQ(numbers_2, a.getNumbers());
  EXPECT_EQ(numbers_1, b.getNumbers());
}
