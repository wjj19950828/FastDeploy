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
#include <pybind11/stl.h>
#include "fastdeploy/pybind/main.h"

namespace fastdeploy {
void BindPPOCRSystemv3(pybind11::module& m) {
  // OCRSys
  pybind11::class_<application::ocrsystem::PPOCRSystemv3, FastDeployModel>(
      m, "PPOCRSystemv3")

      .def(pybind11::init<fastdeploy::vision::ocr::DBDetector*,
                          fastdeploy::vision::ocr::Classifier*,
                          fastdeploy::vision::ocr::Recognizer*>())
      .def(pybind11::init<fastdeploy::vision::ocr::DBDetector*,
                          fastdeploy::vision::ocr::Recognizer*>())
      .def("predict", [](application::ocrsystem::PPOCRSystemv3& self,
                         pybind11::array& data) {
        auto mat = PyArrayToCvMat(data);
        vision::OCRResult res;
        self.Predict(&mat, &res);
        return res;
      });
}

void BindPPOCRSystemv2(pybind11::module& m) {
  // OCRSys
  pybind11::class_<application::ocrsystem::PPOCRSystemv2, FastDeployModel>(
      m, "PPOCRSystemv2")
      .def(pybind11::init<fastdeploy::vision::ocr::DBDetector*,
                          fastdeploy::vision::ocr::Classifier*,
                          fastdeploy::vision::ocr::Recognizer*>())
      .def(pybind11::init<fastdeploy::vision::ocr::DBDetector*,
                          fastdeploy::vision::ocr::Recognizer*>())
      .def("predict", [](application::ocrsystem::PPOCRSystemv2& self,
                         pybind11::array& data) {
        auto mat = PyArrayToCvMat(data);
        vision::OCRResult res;
        self.Predict(&mat, &res);
        return res;
      });
}

}  // namespace fastdeploy
