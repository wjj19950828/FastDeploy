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

#include "fastdeploy/vision/common/processors/transform.h"

namespace fastdeploy {
namespace vision {

void FuseNormalizeCast(
    std::vector<std::shared_ptr<Processor>>* processors) {
  // Fuse Normalize and Cast<Float>
  int cast_index = -1;
  for (size_t i = 0; i < processors->size(); ++i) {
    if ((*processors)[i]->Name() == "Cast") {
      if (i == 0) {
        continue;
      }
      if ((*processors)[i - 1]->Name() != "Normalize" &&
          (*processors)[i - 1]->Name() != "NormalizeAndPermute") {
        continue;
      }
      cast_index = i;
    }
  }
  if (cast_index < 0) {
    return;
  }

  if (dynamic_cast<Cast*>((*processors)[cast_index].get())->GetDtype() !=
      "float") {
    return;
  }
  processors->erase(processors->begin() + cast_index);
  FDINFO << (*processors)[cast_index - 1]->Name() << " and Cast are fused to "
         << (*processors)[cast_index - 1]->Name()
         << " in preprocessing pipeline." << std::endl;
}

void FuseNormalizeHWC2CHW(
    std::vector<std::shared_ptr<Processor>>* processors) {
  // Fuse Normalize and HWC2CHW to NormalizeAndPermute
  int hwc2chw_index = -1;
  for (size_t i = 0; i < processors->size(); ++i) {
    if ((*processors)[i]->Name() == "HWC2CHW") {
      if (i == 0) {
        continue;
      }
      if ((*processors)[i - 1]->Name() != "Normalize") {
        continue;
      }
      hwc2chw_index = i;
    }
  }

  if (hwc2chw_index < 0) {
    return;
  }

  // Get alpha and beta of Normalize
  std::vector<float> alpha =
      dynamic_cast<Normalize*>((*processors)[hwc2chw_index - 1].get())
          ->GetAlpha();
  std::vector<float> beta =
      dynamic_cast<Normalize*>((*processors)[hwc2chw_index - 1].get())
          ->GetBeta();

  // Delete Normalize and HWC2CHW
  processors->erase(processors->begin() + hwc2chw_index);
  processors->erase(processors->begin() + hwc2chw_index - 1);

  // Add NormalizeAndPermute
  std::vector<float> mean({0.0, 0.0, 0.0});
  std::vector<float> std({1.0, 1.0, 1.0});
  processors->push_back(std::make_shared<NormalizeAndPermute>(mean, std));

  // Set alpha and beta
  auto processor = dynamic_cast<NormalizeAndPermute*>(
      (*processors)[hwc2chw_index - 1].get());

  processor->SetAlpha(alpha);
  processor->SetBeta(beta);
  FDINFO << "Normalize and HWC2CHW are fused to NormalizeAndPermute "
            " in preprocessing pipeline."
         << std::endl;
}

void FuseTransforms(
    std::vector<std::shared_ptr<Processor>>* processors) {
  FuseNormalizeCast(processors);
  FuseNormalizeHWC2CHW(processors);
}


}  // namespace vision
}  // namespace fastdeploy
