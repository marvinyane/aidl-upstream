/*
 * Copyright (C) 2015, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AIDL_IO_DELEGATE_H_
#define AIDL_IO_DELEGATE_H_

#include <base/macros.h>

#include <memory>
#include <string>

namespace android {
namespace aidl {

class IoDelegate {
 public:
  IoDelegate() = default;
  virtual ~IoDelegate() = default;

  // Returns a unique_ptr to the contents of |filename|.
  // Will append the optional |content_suffix| to the returned contents.
  virtual std::unique_ptr<std::string> GetFileContents(
      const std::string& filename,
      const std::string& content_suffix = "") const;

  virtual bool FileIsReadable(const std::string& path) const;

 private:
  DISALLOW_COPY_AND_ASSIGN(IoDelegate);
};  // class IoDelegate

}  // namespace android
}  // namespace aidl

#endif // AIDL_IO_DELEGATE_H_
