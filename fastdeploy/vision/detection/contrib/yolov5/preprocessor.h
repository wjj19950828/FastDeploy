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

#pragma once
#include "fastdeploy/vision/common/processors/transform.h"
#include "fastdeploy/vision/common/result.h"

namespace fastdeploy {
namespace vision {

namespace detection {
/*! @brief Preprocessor object for YOLOv5 serials model.
 */
class FASTDEPLOY_DECL YOLOv5Preprocessor {
 public:
  /** \brief Create a preprocessor instance for YOLOv5 serials model
   */
  YOLOv5Preprocessor();

  /** \brief Process the input image and prepare input tensors for runtime
   *
   * \param[in] images The input image data list, all the elements are returned by cv::imread()
   * \param[in] outputs The output tensors which will feed in runtime
   * \param[in] im_info record input_shape and output_shape
   * \return true if the preprocess successed, otherwise false
   */
  bool Run(std::vector<FDMat>* images, std::vector<FDTensor>* outputs,
           std::map<std::string, std::array<float, 2>>* im_info);

  /// Set target size, tuple of (width, height), default size = {640, 640}
  void SetSize(std::vector<int> size) { size_ = size; }

  /// Get target size, tuple of (width, height), default size = {640, 640}
  void GetSize() const { return size_; }

  /// Set padding value, size should be the same as channels
  void SetPaddingValue(std::vector<float> padding_value) {
    padding_value_ = padding_value;
  }

  /// Get padding value, size should be the same as channels
  void GetPaddingValue() const { return padding_value_; }

 private:
  bool Preprocess(FDMat* mat, FDTensor* output,
                  std::map<std::string, std::array<float, 2>>* im_info);

  void UseCudaPreprocessing(int max_img_size = 3840 * 2160);

  bool CudaPreprocess(FDMat* mat, FDTensor* output,
                      std::map<std::string, std::array<float, 2>>* im_info);

  bool IsDynamicInput() const { return is_dynamic_input_; }

  void LetterBox(FDMat* mat);

  bool initialized_ = false;
  // target size, tuple of (width, height), default size = {640, 640}
  std::vector<int> size_;

  // padding value, size should be the same as channels
  std::vector<float> padding_value_;

  // only pad to the minimum rectange which height and width is times of stride
  bool is_mini_pad_;

  // while is_mini_pad = false and is_no_pad = true,
  // will resize the image to the set size
  bool is_no_pad_;

  // if is_scale_up is false, the input image only can be zoom out,
  // the maximum resize scale cannot exceed 1.0
  bool is_scale_up_;

  // padding stride, for is_mini_pad
  int stride_;

  // for offseting the boxes by classes when using NMS
  float max_wh_;

  // whether to inference with dynamic shape (e.g ONNX export with dynamic shape
  // or not.)
  // YOLOv5 official 'export_onnx.py' script will export dynamic ONNX by
  // default.
  // while is_dynamic_shape if 'false', is_mini_pad will force 'false'. This
  // value will
  // auto check by fastdeploy after the internal Runtime already initialized.
  bool is_dynamic_input_;
  // CUDA host buffer for input image
  uint8_t* input_img_cuda_buffer_host_ = nullptr;
  // CUDA device buffer for input image
  uint8_t* input_img_cuda_buffer_device_ = nullptr;
  // CUDA device buffer for TRT input tensor
  float* input_tensor_cuda_buffer_device_ = nullptr;
  // Whether to use CUDA preprocessing
  bool use_cuda_preprocessing_ = false;
  // CUDA stream
  void* cuda_stream_ = nullptr;
};

}  // namespace detection
}  // namespace vision
}  // namespace fastdeploy
