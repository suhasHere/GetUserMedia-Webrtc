/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// Original author: ekr@rtfm.com
#include <iostream>

#include "nsThreadUtils.h"
#include "nsXPCOM.h"
#include "nsStaticComponents.h"


// nrappkit includes
extern "C" {
#include "nr_api.h"
#include "async_timer.h"
}

#include "mtransport_test_utils.h"
#include "runnable_utils.h"

#define GTEST_HAS_RTTI 0
#include "gtest/gtest.h"
#include "gtest_utils.h"

MtransportTestUtils test_utils;

namespace {

class TimerTest : public ::testing::Test {
 public:
  TimerTest() : handle_(NULL), fired_(false) {}

  int ArmTimer(int timeout) {
    int ret;

    test_utils.sts_target()->Dispatch(
        WrapRunnableRet(this, &TimerTest::ArmTimer_w, timeout, &ret),
        NS_DISPATCH_SYNC);
    
    return ret;
  }

  int ArmTimer_w(int timeout) {
    return NR_ASYNC_TIMER_SET(timeout, cb, this, &handle_);
  }

  static void cb(NR_SOCKET r, int how, void *arg) {
    std::cerr << "Timer fired " << std::endl;

    TimerTest *t = static_cast<TimerTest *>(arg);

    t->fired_ = true;
  }

 protected:
  void *handle_;
  bool fired_;
};
}

TEST_F(TimerTest, SimpleTimer) {
  ArmTimer(100);
  ASSERT_TRUE_WAIT(fired_, 1000);
}

int main(int argc, char **argv)
{
  test_utils.InitServices();

  // Start the tests
  ::testing::InitGoogleTest(&argc, argv);
  
  return RUN_ALL_TESTS();
}


// Defining this here, usually generated for libxul
// should not be needed by these tests
const mozilla::Module *const *const kPStaticModules[] = {
  NULL
};


