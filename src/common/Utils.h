/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include <arrayfire.h>
#include <flashlight/flashlight.h>

#include "common/Defines.h"
#include "common/Dictionary.h"
#include "common/Utils-base.h"

namespace w2l {

template <typename T>
std::vector<T> afToVector(const af::array& arr) {
  std::vector<T> vec(arr.elements());
  arr.host(vec.data());
  return vec;
}

// Converts an integer array to corresponding ascii representation.
// "-1"s are ignored.
template <>
inline std::vector<std::string> afToVector(const af::array& arr) {
  auto maxLen = arr.dims(0);
  auto batchSz = arr.dims(1);
  auto intVec = afToVector<int>(arr);

  std::vector<std::string> vec(batchSz);
  std::vector<char> charVec(maxLen);
  int curLen;
  for (int b = 0; b < batchSz; ++b) {
    auto offset = maxLen * b;
    for (curLen = 0; curLen < maxLen; ++curLen) {
      if (intVec[offset + curLen] == -1) {
        break;
      }
      charVec[curLen] = static_cast<char>(intVec[offset + curLen]);
    }
    vec[b] = std::string(charVec.begin(), charVec.begin() + curLen);
  }
  return vec;
}

template <typename T>
std::vector<T> afToVector(const fl::Variable& var) {
  return afToVector<T>(var.array());
}

template <
    typename FwdIt,
    typename = fl::cpp::enable_if_t<std::is_same<
        fl::cpp::decay_t<decltype(*std::declval<FwdIt>())>,
        std::string>::value>>
std::string join(const std::string& delim, FwdIt begin, FwdIt end);

std::string join(const std::string& delim, const std::vector<std::string>& vec);

template <class... Args>
std::string format(const char* fmt, Args&&... args);

int64_t numTotalParams(std::shared_ptr<fl::Module> module);

struct EmissionSet {
  std::vector<std::vector<float>> emissions;
  std::vector<std::vector<int>> wordTargets;
  std::vector<std::vector<int>> letterTargets;
  std::vector<std::string> sampleIds;
  std::vector<float> transition;
  std::vector<int> emissionT;
  int emissionN; // Assume alphabet size to be identical for all the samples

  std::string gflags; // Saving all the flags used in model training

  FL_SAVE_LOAD(
      emissions,
      wordTargets,
      letterTargets,
      sampleIds,
      transition,
      emissionT,
      emissionN,
      gflags)
};

} // namespace w2l

#include "Utils-inl.h"
