// Copyright (c) 2022 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "fastdeploy/vision/common/processors/base.h"
#include "fastdeploy/vision/common/processors/proc_lib.h"

#include "fastdeploy/utils/utils.h"

namespace fastdeploy {
namespace vision {

bool Processor::operator()(Mat* mat, ProcLib lib) {
  ProcLib target = lib;
  if (lib == ProcLib::DEFAULT) {
    target = DefaultProcLib::default_lib;
  }
  if (target == ProcLib::FLYCV) {
#ifdef ENABLE_FLYCV
    return ImplByFlyCV(mat);
#else
    FDASSERT(false, "FastDeploy didn't compile with FlyCV.");
#endif
  }
  // DEFAULT & OPENCV
  return ImplByOpenCV(mat);
}

void EnableFlyCV() {
#ifdef ENABLE_FLYCV
  DefaultProcLib::default_lib = ProcLib::FLYCV;
  FDINFO << "Will change to use image processing library "
         << DefaultProcLib::default_lib << std::endl;
#else
  FDWARNING << "FastDeploy didn't compile with FlyCV, "
               "will fallback to use OpenCV instead."
            << std::endl;
#endif
}

void DisableFlyCV() {
  DefaultProcLib::default_lib = ProcLib::OPENCV;
  FDINFO << "Will change to use image processing library "
         << DefaultProcLib::default_lib << std::endl;
}

void SetProcLibCpuNumThreads(int threads) {
  cv::setNumThreads(threads);
#ifdef ENABLE_FLYCV
  fcv::set_thread_num(threads);
#endif
}

}  // namespace vision
}  // namespace fastdeploy
