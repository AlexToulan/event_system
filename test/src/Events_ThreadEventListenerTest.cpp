#include <thread>

#include "gtest/gtest.h"

#include "ProcessDataListener.hpp"
#include "ThreadEventListener.hpp"
#include "TestEventSystem.hpp"

std::unique_ptr<EventSystem<ETestType>> Events::Test;

/// @brief Testing sending and receiving data between threads
class Events_ThreadEventListenerTest : public testing::Test {
protected:
  void SetUp() override {
    Events::Test = std::make_unique<EventSystem<ETestType>>();
    numbers = {5, 4, 3, 2, 1};
    worker.start();
  };

  void TearDown() override {
    worker.stop();
  };

  ProcessDataListener requester;
  ThreadEventListener worker;
  std::vector<int> numbers;
};

TEST_F(Events_ThreadEventListenerTest, ThreadWorkerTest) {
  Events::Test->bind(ETestType::VECTOR_INT, &requester, &ProcessDataListener::receivedNumbers);
  Events::Test->bind(ETestType::REQ_DOUBLE_INTS, &worker, &ThreadEventListener::receiveNumbers);
  requester.setOutNumbers(numbers);

  requester.send();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(numbers.size(), requester.getNumbers().size());
  for(int i = 0; i < numbers.size(); i++) {
    EXPECT_EQ(numbers[i] * 2, requester.getNumbers()[i]);
  }

  requester.clearInNumbers();
  requester.send();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(numbers.size(), requester.getNumbers().size());
  for(int i = 0; i < numbers.size(); i++) {
    EXPECT_EQ(numbers[i] * 2, requester.getNumbers()[i]);
  }
}
