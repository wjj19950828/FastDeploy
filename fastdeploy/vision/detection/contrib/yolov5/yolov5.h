﻿// Copyright (c) 2022 PaddlePaddle Authors. All Rights Reserved.  //NOLINT
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

#pragma once

#include "fastdeploy/fastdeploy_model.h"
#include "fastdeploy/vision/detection/contrib/yolov5/preprocessor.h"
#include "fastdeploy/vision/detection/contrib/yolov5/postprocessor.h"

namespace fastdeploy {
namespace vision {
namespace detection {
/*! @brief YOLOv5 model object used when to load a YOLOv5 model exported by YOLOv5.
 */
class FASTDEPLOY_DECL YOLOv5 : public FastDeployModel {
 public:
  /** \brief  Set path of model file and the configuration of runtime.
   *
   * \param[in] model_file Path of model file, e.g ./yolov5.onnx
   * \param[in] params_file Path of parameter file, e.g ppyoloe/model.pdiparams, if the model format is ONNX, this parameter will be ignored
   * \param[in] custom_option RuntimeOption for inference, the default will use cpu, and choose the backend defined in "valid_cpu_backends"
   * \param[in] model_format Model format of the loaded model, default is ONNX format
   */
  YOLOv5(const std::string& model_file, const std::string& params_file = "",
         const RuntimeOption& custom_option = RuntimeOption(),
         const ModelFormat& model_format = ModelFormat::ONNX);

  ~YOLOv5();

  std::string ModelName() const { return "yolov5"; }

  /** \brief Predict the detection result for an input image
   *
   * \param[in] im The input image data, comes from cv::imread(), is a 3-D array with layout HWC, BGR format
   * \param[in] result The output detection result will be writen to this structure
   * \param[in] conf_threshold confidence threashold for postprocessing, default is 0.25
   * \param[in] nms_iou_threshold iou threashold for NMS, default is 0.5
   * \return true if the prediction successed, otherwise false
   */
  virtual bool Predict(cv::Mat* im, DetectionResult* result,
                       float conf_threshold = 0.25,
                       float nms_iou_threshold = 0.5);

  /** \brief Predict the detection result for an input image
   *
   * \param[in] img The input image data, comes from cv::imread(), is a 3-D array with layout HWC, BGR format
   * \param[in] result The output detection result will be writen to this structure
   * \param[in] conf_threshold confidence threashold for postprocessing, default is 0.25
   * \param[in] nms_iou_threshold iou threashold for NMS, default is 0.5
   * \return true if the prediction successed, otherwise false
   */
  virtual bool Predict(const cv::Mat& img, DetectionResult* result,
                       float conf_threshold = 0.25,
                       float nms_iou_threshold = 0.5);

  /** \brief Predict the detection results for a batch of input images
   *
   * \param[in] imgs, The input image list, each element comes from cv::imread()
   * \param[in] results The output classification result list
   * \param[in] conf_threshold confidence threashold for postprocessing, default is 0.25
   * \param[in] nms_iou_threshold iou threashold for NMS, default is 0.5
   * \return true if the prediction successed, otherwise false
   */
  virtual bool BatchPredict(const std::vector<cv::Mat>& imgs,
                            std::vector<DetectionResult>* results,
                            float conf_threshold = 0.25,
                            float nms_iou_threshold = 0.5);

  /// Get preprocessor reference of PaddleClasModel
  virtual PaddleClasPreprocessor& GetPreprocessor() {
    return preprocessor_;
  }

  /// Get postprocessor reference of PaddleClasModel
  virtual PaddleClasPostprocessor& GetPostprocessor() {
    return postprocessor_;
  }

 protected:
  bool Initialize();
  YOLOv5Preprocessor preprocessor_;
  YOLOv5Postprocessor postprocessor_;
};

}  // namespace detection
}  // namespace vision
}  // namespace fastdeploy
