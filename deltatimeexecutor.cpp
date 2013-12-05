/**
 * @file deltatimeexecutor.cpp
 * @date 04.12.2013
 */

#include "deltatimeexecutor.h"

DeltaTimeExecutor::DeltaTimeExecutor(Callback callback,
                                     Interval minimumInterval)
: clock_(), minimumInterval_(minimumInterval), lastTime_(clock_.now()),
  callback_(callback) {}

void DeltaTimeExecutor::callIfTimeOver() {
  if (clock_.now() - lastTime_ > minimumInterval_) {
    // reset clock before the callback to avoid reentrance when callback
    // triggers callIfTimeOver
    lastTime_ = clock_.now();
    callback_();
  }
}
