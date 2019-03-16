/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INCLUDE_PERFETTO_BASE_THREAD_TASK_RUNNER_H_
#define INCLUDE_PERFETTO_BASE_THREAD_TASK_RUNNER_H_

#include <functional>
#include <thread>

#include "perfetto/base/unix_task_runner.h"

namespace perfetto {
namespace base {

// A UnixTaskRunner backed by a dedicated task thread. Shuts down the runner and
// joins the thread upon destruction. Can be moved to transfer ownership.
//
// Guarantees that:
// * the UnixTaskRunner will be constructed and destructed on the task thread.
// * the task thread will live for the lifetime of the UnixTaskRunner.
//
class ThreadTaskRunner {
 public:
  static ThreadTaskRunner CreateAndStart() { return ThreadTaskRunner(); }

  ThreadTaskRunner(const ThreadTaskRunner&) = delete;
  ThreadTaskRunner& operator=(const ThreadTaskRunner&) = delete;

  ThreadTaskRunner(ThreadTaskRunner&&) noexcept;
  ThreadTaskRunner& operator=(ThreadTaskRunner&&);
  ~ThreadTaskRunner();

  // Returns a pointer to the UnixTaskRunner, which is valid for the lifetime of
  // this ThreadTaskRunner object (unless this object is moved-from, in which
  // case the pointer remains valid for the lifetime of the new owning
  // ThreadTaskRunner).
  //
  // Warning: do not call Quit() on the returned runner pointer, the termination
  // should be handled exclusively by this class' destructor.
  UnixTaskRunner* get() { return task_runner_; }

 private:
  ThreadTaskRunner();
  void RunTaskThread(std::function<void(UnixTaskRunner*)> initializer);

  std::thread thread_;
  UnixTaskRunner* task_runner_ = nullptr;
};

}  // namespace base
}  // namespace perfetto

#endif  // INCLUDE_PERFETTO_BASE_THREAD_TASK_RUNNER_H_
