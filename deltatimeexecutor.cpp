/**
 * @file deltatimeexecutor.cpp
 * @date 04.12.2013
 */

#include "deltatimeexecutor.h"

DeltaTimeExecutor::DeltaTimeExecutor(Callback callback,
                                     Interval minimumInterval)
: clock_(), minimumInterval_(minimumInterval), lastTime_(),
  callback_(callback) {}

void DeltaTimeExecutor::callIfTimeOver() {
  if (!lastTime_ || *lastTime_ - clock_.now() > minimumInterval_) {
    callback_();
    lastTime_ = clock_.now();
  }
}
