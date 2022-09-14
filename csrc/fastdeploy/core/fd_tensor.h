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

#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "fastdeploy/core/allocate.h"
#include "fastdeploy/core/fd_type.h"

namespace fastdeploy {

struct FASTDEPLOY_DECL FDTensor {
  // std::vector<int8_t> data;
  void* buffer_ = nullptr;
  std::vector<int64_t> shape = {0};
  std::string name = "";
  FDDataType dtype = FDDataType::INT8;

  // This use to skip memory copy step
  // the external_data_ptr will point to the user allocated memory
  // user has to maintain the memory, allocate and release
  void* external_data_ptr = nullptr;
  // The internal data will be on CPU
  // Some times, the external data is on the GPU, and we are going to use
  // GPU to inference the model
  // so we can skip data transfer, which may improve the efficience
  Device device = Device::CPU;

  // if the external data is not on CPU, we use this temporary buffer
  // to transfer data to CPU at some cases we need to visit the
  // other devices' data
  std::vector<int8_t> temporary_cpu_buffer;

  // Get data buffer pointer
  void* MutableData();

  void* Data();

  const void* Data() const;

  // Use this data to get the tensor data to process
  // Since the most senario is process data in CPU
  // this function will return a pointer to cpu memory
  // buffer.
  // If the original data is on other device, the data
  // will copy to cpu store in `temporary_cpu_buffer`
  const void* CpuData() const;

  // Set user memory buffer for Tensor, the memory is managed by
  // the user it self, but the Tensor will share the memory with user
  // So take care with the user buffer
  void SetExternalData(const std::vector<int64_t>& new_shape,
                       const FDDataType& data_type, void* data_buffer,
                       const Device& new_device = Device::CPU);

  // Initialize Tensor
  // Include setting attribute for tensor
  // and allocate cpu memory buffer
  void Allocate(const std::vector<int64_t>& new_shape,
                const FDDataType& data_type,
                const std::string& tensor_name = "",
                const Device& new_device = Device::CPU);

  // Total size of tensor memory buffer in bytes
  int Nbytes() const;

  // Total number of elements in this tensor
  int Numel() const;

  void Resize(size_t nbytes);

  void Resize(const std::vector<int64_t>& new_shape);

  void Resize(const std::vector<int64_t>& new_shape,
              const FDDataType& data_type, const std::string& tensor_name = "",
              const Device& new_device = Device::CPU);

  // Debug function
  // Use this function to print shape, dtype, mean, max, min
  // prefix will also be printed as tag
  void PrintInfo(const std::string& prefix = "TensorInfo: ");

  bool AllocFn(size_t nbytes) {
    if (device == Device::GPU) {
#ifdef WITH_GPU
      return FDDeviceAllocator()(&buffer_, nbytes);
#else
      FDASSERT(false,
               "The FastDeploy FDTensor allocator didn't compile under "
               "-DWITH_GPU=ON,"
               "so this is an unexpected problem happend.");
#endif
    }
    return FDHostAllocator()(&buffer_, nbytes);
  }

  void FreeFn() {
    if (external_data_ptr != nullptr) external_data_ptr = nullptr;
    if (buffer_ != nullptr) {
      if (device == Device::GPU) {
#ifdef WITH_GPU
        FDDeviceFree()(buffer_);
#endif
      } else {
        FDHostFree()(buffer_);
      }
      buffer_ = nullptr;
    }
  }

  FDTensor() {}
  explicit FDTensor(const std::string& tensor_name);

  ~FDTensor() { FreeFn(); }
};

}  // namespace fastdeploy
