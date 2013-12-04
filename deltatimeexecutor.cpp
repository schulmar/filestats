/*
 * deltatimeexecutor.cpp
 *
 *  Created on: 04.12.2013
 *      Author: martin
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
