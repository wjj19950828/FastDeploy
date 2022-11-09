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

#include "fastdeploy/vision/common/processors/cast.h"

namespace fastdeploy {
namespace vision {

bool Cast::ImplByOpenCV(Mat* mat) {
  cv::Mat* im = mat->GetOpenCVMat();
  int c = im->channels();
  if (dtype_ == "float") {
    if (im->type() != CV_32FC(c)) {
      im->convertTo(*im, CV_32FC(c));
    }
  } else if (dtype_ == "double") {
    if (im->type() != CV_64FC(c)) {
      im->convertTo(*im, CV_64FC(c));
    }
  } else {
    FDWARNING << "Cast not support for " << dtype_
              << " now! will skip this operation." << std::endl;
  }
  return true;
}

#ifdef ENABLE_FLYCV
bool Cast::ImplByFlyCV(Mat* mat) {
  fcv::Mat* im = mat->GetFlyCVMat();
  if (dtype_ == "float" && mat->Type() == FDDataType::FP32) {
    return true;
  }
  if (dtype_ == "double" && mat->Type() == FDDataType::FP64) {
    return true;
  }
  if (mat->layout != Layout::HWC) {
    FDERROR
        << "While using FlyCV to cast image, the image must be layout of HWC."
        << std::endl;
    return false;
  }
  if (dtype_ == "float") {
    fcv::Mat new_im;
    auto fcv_type = CreateFlyCVDataType(FDDataType::FP32, im->channels());
    im->convert_to(new_im, fcv_type);
    mat->SetMat(new_im);
  } else if (dtype_ == "double") {
    fcv::Mat new_im;
    auto fcv_type = CreateFlyCVDataType(FDDataType::FP64, im->channels());
    im->convert_to(new_im, fcv_type);
    mat->SetMat(new_im);
  } else {
    FDWARNING << "Cast not support for " << dtype_
              << " now! will skip this operation." << std::endl;
  }
  return true;
}
#endif

bool Cast::Run(Mat* mat, const std::string& dtype, ProcLib lib) {
  auto c = Cast(dtype);
  return c(mat, lib);
}

}  // namespace vision
}  // namespace fastdeploy
