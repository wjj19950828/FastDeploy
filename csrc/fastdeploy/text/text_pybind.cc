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

#include "fastdeploy/pybind/main.h"

namespace fastdeploy {

void BindUIE(pybind11::module& m);

void BindText(pybind11::module& m) {
  pybind11::class_<text::UIEResult>(m, "UIEResult")
      .def(pybind11::init())
      .def_readwrite("start", &text::UIEResult::start_)
      .def_readwrite("end", &text::UIEResult::end_)
      .def_readwrite("probability_", &text::UIEResult::probability_)
      .def_readwrite("text", &text::UIEResult::text_)
      .def_readwrite("relation", &text::UIEResult::relation_)
      .def("__repr__", &text::UIEResult::Str)
      .def("__str__", &text::UIEResult::Str);
  BindUIE(m);
}

}  // namespace fastdeploy
