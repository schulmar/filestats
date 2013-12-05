/*
 * destructionexecutor.h
 *
 *  Created on: 04.12.2013
 *      Author: martin
 */

#ifndef DESTRUCTIONEXECUTOR_H_
#define DESTRUCTIONEXECUTOR_H_

/**
 * @brief Calls the given callback on destruction
 */
struct DestructionExecutor {
  typedef std::function<void()> Callback;

  DestructionExecutor(Callback callback)
  : callback_(callback)
  {}

  ~DestructionExecutor() {
    callback_();
  }
private:
  Callback callback_;
};

#endif /* DESTRUCTIONEXECUTOR_H_ */
