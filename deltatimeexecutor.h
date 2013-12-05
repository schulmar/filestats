/**
 * @file deltatimeexecutor.h
 * @date 04.12.2013
 */

#ifndef DELTATIMEEXECUTOR_H_
#define DELTATIMEEXECUTOR_H_

#include <cstdint>
#include <functional>
#include <chrono>

/**
 * @brief Executes a callback in given intervals if called
 */
struct DeltaTimeExecutor {
  typedef std::function<void()> Callback;
  typedef std::chrono::high_resolution_clock Clock;
  typedef std::chrono::time_point<Clock> Timestamp;
  typedef Clock::duration Interval;

  /**
   * @param callback Function to call
   * @param minimumInterval minimal time interval between two function calls
   *
   * The callback will not be called on construction, but the interval starts
   * now
   */
  DeltaTimeExecutor(Callback callback, Interval minimumInterval);

  /**
   * @brief Calls the callback if the time interval between the last call and
   *        this call is bigger than the minimum given in the constructor
   */
  void callIfTimeOver();

private:
  /// the clock used for timing
  Clock clock_;
  /// the minimal time interval between two calls of the callback
  Interval minimumInterval_;
  /// the last time we executed the callback
  Timestamp lastTime_;
  /// the callback which should be called
  Callback callback_;
};

#endif /* DELTATIMEEXECUTOR_H_ */
