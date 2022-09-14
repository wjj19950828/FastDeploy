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

#include "fastdeploy/fastdeploy_runtime.h"
#include "fastdeploy/utils/unique_ptr.h"
#include "fastdeploy/utils/utils.h"

#ifdef ENABLE_ORT_BACKEND
#include "fastdeploy/backends/ort/ort_backend.h"
#endif

#ifdef ENABLE_TRT_BACKEND
#include "fastdeploy/backends/tensorrt/trt_backend.h"
#endif

#ifdef ENABLE_PADDLE_BACKEND
#include "fastdeploy/backends/paddle/paddle_backend.h"
#endif

#ifdef ENABLE_POROS_BACKEND
#include "fastdeploy/backends/poros/poros_backend.h"
#endif

#ifdef ENABLE_OPENVINO_BACKEND
#include "fastdeploy/backends/openvino/ov_backend.h"
#endif

namespace fastdeploy {

std::vector<Backend> GetAvailableBackends() {
  std::vector<Backend> backends;
#ifdef ENABLE_ORT_BACKEND
  backends.push_back(Backend::ORT);
#endif
#ifdef ENABLE_TRT_BACKEND
  backends.push_back(Backend::TRT);
#endif
#ifdef ENABLE_PADDLE_BACKEND
  backends.push_back(Backend::PDINFER);
#endif
#ifdef ENABLE_POROS_BACKEND
  backends.push_back(Backend::POROS);
#endif
#ifdef ENABLE_OPENVINO_BACKEND
  backends.push_back(Backend::OPENVINO);
#endif
  return backends;
}

bool IsBackendAvailable(const Backend& backend) {
  std::vector<Backend> backends = GetAvailableBackends();
  for (size_t i = 0; i < backends.size(); ++i) {
    if (backend == backends[i]) {
      return true;
    }
  }
  return false;
}

std::string Str(const Backend& b) {
  if (b == Backend::ORT) {
    return "Backend::ORT";
  } else if (b == Backend::TRT) {
    return "Backend::TRT";
  } else if (b == Backend::PDINFER) {
    return "Backend::PDINFER";
  } else if (b == Backend::POROS) {
    return "Backend::POROS";
  } else if (b == Backend::OPENVINO) {
    return "Backend::OPENVINO";
  }
  return "UNKNOWN-Backend";
}

std::string Str(const Frontend& f) {
  if (f == Frontend::PADDLE) {
    return "Frontend::PADDLE";
  } else if (f == Frontend::ONNX) {
    return "Frontend::ONNX";
  } else if (f == Frontend::TORCHSCRIPT) {
    return "Frontend::TORCHSCRIPT";
  }
  return "UNKNOWN-Frontend";
}

bool CheckModelFormat(const std::string& model_file,
                      const Frontend& model_format) {
  if (model_format == Frontend::PADDLE) {
    if (model_file.size() < 8 ||
        model_file.substr(model_file.size() - 8, 8) != ".pdmodel") {
      FDERROR << "With model format of Frontend::PADDLE, the model file "
                 "should ends with `.pdmodel`, but now it's "
              << model_file << std::endl;
      return false;
    }
  } else if (model_format == Frontend::ONNX) {
    if (model_file.size() < 5 ||
        model_file.substr(model_file.size() - 5, 5) != ".onnx") {
      FDERROR << "With model format of Frontend::ONNX, the model file "
                 "should ends with `.onnx`, but now it's "
              << model_file << std::endl;
      return false;
    }
  } else if (model_format == Frontend::TORCHSCRIPT) {
    if (model_file.size() < 3 ||
        model_file.substr(model_file.size() - 3, 3) != ".pt") {
      FDERROR << "With model format of Frontend::TORCHSCRIPT, the model file "
                 "should ends with `.pt`, but now it's "
              << model_file << std::endl;
      return false;
    }
  } else {
    FDERROR << "Only support model format with frontend Frontend::PADDLE / "
               "Frontend::ONNX / Frontend::TORCHSCRIPT."
            << std::endl;
    return false;
  }
  return true;
}

Frontend GuessModelFormat(const std::string& model_file) {
  if (model_file.size() > 8 &&
      model_file.substr(model_file.size() - 8, 8) == ".pdmodel") {
    FDLogger() << "Model Format: PaddlePaddle." << std::endl;
    return Frontend::PADDLE;
  } else if (model_file.size() > 5 &&
             model_file.substr(model_file.size() - 5, 5) == ".onnx") {
    FDLogger() << "Model Format: ONNX." << std::endl;
    return Frontend::ONNX;
  } else if (model_file.size() > 3 &&
             model_file.substr(model_file.size() - 3, 3) == ".pt") {
    FDLogger() << "Model Format: Torchscript." << std::endl;
    return Frontend::TORCHSCRIPT;
  }

  FDERROR << "Cannot guess which model format you are using, please set "
             "RuntimeOption::model_format manually."
          << std::endl;
  return Frontend::PADDLE;
}

void RuntimeOption::SetModelPath(const std::string& model_path,
                                 const std::string& params_path,
                                 const std::string& _model_format) {
  if (_model_format == "paddle") {
    model_file = model_path;
    params_file = params_path;
    model_format = Frontend::PADDLE;
  } else if (_model_format == "onnx") {
    model_file = model_path;
    model_format = Frontend::ONNX;
  } else if (_model_format == "torchscript") {
    model_file = model_path;
    model_format = Frontend::TORCHSCRIPT;
  } else {
    FDASSERT(
        false,
        "The model format only can be 'paddle' or 'onnx' or 'torchscript'.");
  }
}

void RuntimeOption::UseGpu(int gpu_id) {
#ifdef WITH_GPU
  device = Device::GPU;
  device_id = gpu_id;
#else
  FDWARNING << "The FastDeploy didn't compile with GPU, will force to use CPU."
            << std::endl;
  device = Device::CPU;
#endif
}

void RuntimeOption::UseCpu() { device = Device::CPU; }

void RuntimeOption::SetCpuThreadNum(int thread_num) {
  FDASSERT(thread_num > 0, "The thread_num must be greater than 0.");
  cpu_thread_num = thread_num;
}

// use paddle inference backend
void RuntimeOption::UsePaddleBackend() {
#ifdef ENABLE_PADDLE_BACKEND
  backend = Backend::PDINFER;
#else
  FDASSERT(false, "The FastDeploy didn't compile with Paddle Inference.");
#endif
}

// use onnxruntime backend
void RuntimeOption::UseOrtBackend() {
#ifdef ENABLE_ORT_BACKEND
  backend = Backend::ORT;
#else
  FDASSERT(false, "The FastDeploy didn't compile with OrtBackend.");
#endif
}

// use poros backend
void RuntimeOption::UsePorosBackend() {
#ifdef ENABLE_POROS_BACKEND
  backend = Backend::POROS;
#else
  FDASSERT(false, "The FastDeploy didn't compile with PorosBackend.");
#endif
}

void RuntimeOption::UseTrtBackend() {
#ifdef ENABLE_TRT_BACKEND
  backend = Backend::TRT;
#else
  FDASSERT(false, "The FastDeploy didn't compile with TrtBackend.");
#endif
}

void RuntimeOption::UseOpenVINOBackend() {
#ifdef ENABLE_OPENVINO_BACKEND
  backend = Backend::OPENVINO;
#else
  FDASSERT(false, "The FastDeploy didn't compile with OpenVINO.");
#endif
}
void RuntimeOption::EnablePaddleMKLDNN() { pd_enable_mkldnn = true; }

void RuntimeOption::DisablePaddleMKLDNN() { pd_enable_mkldnn = false; }

void RuntimeOption::DeletePaddleBackendPass(const std::string& pass_name) {
  pd_delete_pass_names.push_back(pass_name);
}
void RuntimeOption::EnablePaddleLogInfo() { pd_enable_log_info = true; }

void RuntimeOption::DisablePaddleLogInfo() { pd_enable_log_info = false; }

void RuntimeOption::SetPaddleMKLDNNCacheSize(int size) {
  FDASSERT(size > 0, "Parameter size must greater than 0.");
  pd_mkldnn_cache_size = size;
}

void RuntimeOption::SetTrtInputShape(const std::string& input_name,
                                     const std::vector<int32_t>& min_shape,
                                     const std::vector<int32_t>& opt_shape,
                                     const std::vector<int32_t>& max_shape) {
  trt_min_shape[input_name].clear();
  trt_max_shape[input_name].clear();
  trt_opt_shape[input_name].clear();
  trt_min_shape[input_name].assign(min_shape.begin(), min_shape.end());
  if (opt_shape.size() == 0) {
    trt_opt_shape[input_name].assign(min_shape.begin(), min_shape.end());
  } else {
    trt_opt_shape[input_name].assign(opt_shape.begin(), opt_shape.end());
  }
  if (max_shape.size() == 0) {
    trt_max_shape[input_name].assign(min_shape.begin(), min_shape.end());
  } else {
    trt_max_shape[input_name].assign(max_shape.begin(), max_shape.end());
  }
}

void RuntimeOption::EnableTrtFP16() { trt_enable_fp16 = true; }

void RuntimeOption::DisableTrtFP16() { trt_enable_fp16 = false; }

void RuntimeOption::SetTrtCacheFile(const std::string& cache_file_path) {
  trt_serialize_file = cache_file_path;
}

void RuntimeOption::SetInputDtypes(const std::vector<std::string>& dtypes) {
  for (size_t i = 0; i < dtypes.size(); ++i) {
    FDASSERT(dtypes[i] != "float16", "Float16 is not supported!");
    if (dtypes[i] == "float32") {
      input_dtypes.push_back(FDDataType::FP32);
    } else if (dtypes[i] == "float64") {
      input_dtypes.push_back(FDDataType::FP64);
    } else if (dtypes[i] == "int16") {
      input_dtypes.push_back(FDDataType::INT16);
    } else if (dtypes[i] == "int32") {
      input_dtypes.push_back(FDDataType::INT32);
    } else if (dtypes[i] == "int64") {
      input_dtypes.push_back(FDDataType::INT64);
    } else if (dtypes[i] == "bool") {
      input_dtypes.push_back(FDDataType::BOOL);
    } else if (dtypes[i] == "uint8") {
      input_dtypes.push_back(FDDataType::UINT8);
    } else {
      FDASSERT(false, "Unexpected data type: " + dtypes[i]);
    }
  }
}

bool Runtime::Compile(std::vector<std::vector<FDTensor>>& prewarm_tensors,
                      const RuntimeOption& _option) {
#ifdef ENABLE_POROS_BACKEND
  option = _option;
  auto poros_option = PorosBackendOption();
  poros_option.use_gpu = (option.device == Device::GPU) ? true : false;
  poros_option.gpu_id = option.device_id;
  poros_option.long_to_int = option.long_to_int;
  poros_option.use_nvidia_tf32 = option.use_nvidia_tf32;
  poros_option.unconst_ops_thres = option.unconst_ops_thres;
  poros_option.poros_file = option.poros_file;
  poros_option.is_dynamic = option.is_dynamic;
  poros_option.enable_fp16 = option.trt_enable_fp16;
  poros_option.max_batch_size = option.trt_max_batch_size;
  poros_option.max_workspace_size = option.trt_max_workspace_size;
  // poros_option.max_shape = option.trt_max_shape;
  // poros_option.min_shape = option.trt_min_shape;
  // poros_option.opt_shape = option.trt_opt_shape;
  FDASSERT(option.model_format == Frontend::TORCHSCRIPT,
           "PorosBackend only support model format of Frontend::TORCHSCRIPT.");
  backend_ = utils::make_unique<PorosBackend>();
  auto casted_backend = dynamic_cast<PorosBackend*>(backend_.get());
  FDASSERT(
      casted_backend->Compile(option.model_file, prewarm_tensors, poros_option),
      "Load model from Torchscript failed while initliazing PorosBackend.");
#else
  FDASSERT(false,
           "PorosBackend is not available, please compiled with "
           "ENABLE_POROS_BACKEND=ON.");
#endif
  return true;
}

bool Runtime::Init(const RuntimeOption& _option) {
  option = _option;
  if (option.model_format == Frontend::AUTOREC) {
    option.model_format = GuessModelFormat(_option.model_file);
  }
  if (option.backend == Backend::UNKNOWN) {
    if (IsBackendAvailable(Backend::ORT)) {
      option.backend = Backend::ORT;
    } else if (IsBackendAvailable(Backend::PDINFER)) {
      option.backend = Backend::PDINFER;
    } else if (IsBackendAvailable(Backend::POROS)) {
      option.backend = Backend::POROS;
    } else if (IsBackendAvailable(Backend::OPENVINO)) {
      option.backend = Backend::OPENVINO;
    } else {
      FDERROR << "Please define backend in RuntimeOption, current it's "
                 "Backend::UNKNOWN."
              << std::endl;
      return false;
    }
  }

  if (option.backend == Backend::ORT) {
    FDASSERT(option.device == Device::CPU || option.device == Device::GPU,
             "Backend::ORT only supports Device::CPU/Device::GPU.");
    CreateOrtBackend();
    FDINFO << "Runtime initialized with Backend::ORT in device " << Str(option.device) << "." << std::endl;
  } else if (option.backend == Backend::TRT) {
    FDASSERT(option.device == Device::GPU,
             "Backend::TRT only supports Device::GPU.");
    CreateTrtBackend();
    FDINFO << "Runtime initialized with Backend::TRT in device " << Str(option.device) << "." << std::endl;
  } else if (option.backend == Backend::PDINFER) {
    FDASSERT(option.device == Device::CPU || option.device == Device::GPU,
             "Backend::TRT only supports Device::CPU/Device::GPU.");
    FDASSERT(
        option.model_format == Frontend::PADDLE,
        "Backend::PDINFER only supports model format of Frontend::PADDLE.");
    CreatePaddleBackend();
  } else if (option.backend == Backend::POROS) {
    FDASSERT(option.device == Device::CPU || option.device == Device::GPU,
             "Backend::POROS only supports Device::CPU/Device::GPU.");
    FDASSERT(
        option.model_format == Frontend::TORCHSCRIPT,
        "Backend::POROS only supports model format of Frontend::TORCHSCRIPT.");
    // CreatePorosBackend();
    FDINFO << "Runtime initialized with Backend::PDINFER in device " << Str(option.device) << "." << std::endl;
    return true;
  } else if (option.backend == Backend::OPENVINO) {
    FDASSERT(option.device == Device::CPU,
             "Backend::OPENVINO only supports Device::CPU");
    CreateOpenVINOBackend();
    FDINFO << "Runtime initialized with Backend::OPENVINO in device " << Str(option.device) << "." << std::endl;
  } else {
    FDERROR << "Runtime only support "
               "Backend::ORT/Backend::TRT/Backend::PDINFER/Backend::POROS as "
               "backend now."
            << std::endl;
    return false;
  }
  return true;
}

TensorInfo Runtime::GetInputInfo(int index) {
  return backend_->GetInputInfo(index);
}

TensorInfo Runtime::GetOutputInfo(int index) {
  return backend_->GetOutputInfo(index);
}

bool Runtime::Infer(std::vector<FDTensor>& input_tensors,
                    std::vector<FDTensor>* output_tensors) {
  return backend_->Infer(input_tensors, output_tensors);
}

void Runtime::CreatePaddleBackend() {
#ifdef ENABLE_PADDLE_BACKEND
  auto pd_option = PaddleBackendOption();
  pd_option.enable_mkldnn = option.pd_enable_mkldnn;
  pd_option.enable_log_info = option.pd_enable_log_info;
  pd_option.mkldnn_cache_size = option.pd_mkldnn_cache_size;
  pd_option.use_gpu = (option.device == Device::GPU) ? true : false;
  pd_option.gpu_id = option.device_id;
  pd_option.delete_pass_names = option.pd_delete_pass_names;
  pd_option.cpu_thread_num = option.cpu_thread_num;
  FDASSERT(option.model_format == Frontend::PADDLE,
           "PaddleBackend only support model format of Frontend::PADDLE.");
  backend_ = utils::make_unique<PaddleBackend>();
  auto casted_backend = dynamic_cast<PaddleBackend*>(backend_.get());
  FDASSERT(casted_backend->InitFromPaddle(option.model_file, option.params_file,
                                          pd_option),
           "Load model from Paddle failed while initliazing PaddleBackend.");
#else
  FDASSERT(false,
           "PaddleBackend is not available, please compiled with "
           "ENABLE_PADDLE_BACKEND=ON.");
#endif
}

void Runtime::CreateOpenVINOBackend() {
#ifdef ENABLE_OPENVINO_BACKEND
  auto ov_option = OpenVINOBackendOption();
  ov_option.cpu_thread_num = option.cpu_thread_num;
  FDASSERT(option.model_format == Frontend::PADDLE ||
               option.model_format == Frontend::ONNX,
           "OpenVINOBackend only support model format of Frontend::PADDLE / "
           "Frontend::ONNX.");
  backend_ = utils::make_unique<OpenVINOBackend>();
  auto casted_backend = dynamic_cast<OpenVINOBackend*>(backend_.get());

  if (option.model_format == Frontend::ONNX) {
    FDASSERT(casted_backend->InitFromOnnx(option.model_file, ov_option),
             "Load model from ONNX failed while initliazing OrtBackend.");
  } else {
    FDASSERT(casted_backend->InitFromPaddle(option.model_file,
                                            option.params_file, ov_option),
             "Load model from Paddle failed while initliazing OrtBackend.");
  }
#else
  FDASSERT(false,
           "OpenVINOBackend is not available, please compiled with "
           "ENABLE_OPENVINO_BACKEND=ON.");
#endif
}

void Runtime::CreateOrtBackend() {
#ifdef ENABLE_ORT_BACKEND
  auto ort_option = OrtBackendOption();
  ort_option.graph_optimization_level = option.ort_graph_opt_level;
  ort_option.intra_op_num_threads = option.cpu_thread_num;
  ort_option.inter_op_num_threads = option.ort_inter_op_num_threads;
  ort_option.execution_mode = option.ort_execution_mode;
  ort_option.use_gpu = (option.device == Device::GPU) ? true : false;
  ort_option.gpu_id = option.device_id;

  // TODO(jiangjiajun): inside usage, maybe remove this later
  ort_option.remove_multiclass_nms_ = option.remove_multiclass_nms_;
  ort_option.custom_op_info_ = option.custom_op_info_;

  FDASSERT(option.model_format == Frontend::PADDLE ||
               option.model_format == Frontend::ONNX,
           "OrtBackend only support model format of Frontend::PADDLE / "
           "Frontend::ONNX.");
  backend_ = utils::make_unique<OrtBackend>();
  auto casted_backend = dynamic_cast<OrtBackend*>(backend_.get());
  if (option.model_format == Frontend::ONNX) {
    FDASSERT(casted_backend->InitFromOnnx(option.model_file, ort_option),
             "Load model from ONNX failed while initliazing OrtBackend.");
  } else {
    FDASSERT(casted_backend->InitFromPaddle(option.model_file,
                                            option.params_file, ort_option),
             "Load model from Paddle failed while initliazing OrtBackend.");
  }
#else
  FDASSERT(false,
           "OrtBackend is not available, please compiled with "
           "ENABLE_ORT_BACKEND=ON.");
#endif
}

void Runtime::CreateTrtBackend() {
#ifdef ENABLE_TRT_BACKEND
  auto trt_option = TrtBackendOption();
  trt_option.gpu_id = option.device_id;
  trt_option.enable_fp16 = option.trt_enable_fp16;
  trt_option.enable_int8 = option.trt_enable_int8;
  trt_option.max_batch_size = option.trt_max_batch_size;
  trt_option.max_workspace_size = option.trt_max_workspace_size;
  trt_option.max_shape = option.trt_max_shape;
  trt_option.min_shape = option.trt_min_shape;
  trt_option.opt_shape = option.trt_opt_shape;
  trt_option.serialize_file = option.trt_serialize_file;

  // TODO(jiangjiajun): inside usage, maybe remove this later
  trt_option.remove_multiclass_nms_ = option.remove_multiclass_nms_;
  trt_option.custom_op_info_ = option.custom_op_info_;

  FDASSERT(option.model_format == Frontend::PADDLE ||
               option.model_format == Frontend::ONNX,
           "TrtBackend only support model format of Frontend::PADDLE / "
           "Frontend::ONNX.");
  backend_ = utils::make_unique<TrtBackend>();
  auto casted_backend = dynamic_cast<TrtBackend*>(backend_.get());
  if (option.model_format == Frontend::ONNX) {
    FDASSERT(casted_backend->InitFromOnnx(option.model_file, trt_option),
             "Load model from ONNX failed while initliazing TrtBackend.");
  } else {
    FDASSERT(casted_backend->InitFromPaddle(option.model_file,
                                            option.params_file, trt_option),
             "Load model from Paddle failed while initliazing TrtBackend.");
  }
#else
  FDASSERT(false,
           "TrtBackend is not available, please compiled with "
           "ENABLE_TRT_BACKEND=ON.");
#endif
}

void Runtime::CreatePorosBackend() {
#ifdef ENABLE_POROS_BACKEND
  auto poros_option = PorosBackendOption();
  poros_option.use_gpu = (option.device == Device::GPU) ? true : false;
  poros_option.gpu_id = option.device_id;
  poros_option.long_to_int = option.long_to_int;
  poros_option.use_nvidia_tf32 = option.use_nvidia_tf32;
  poros_option.unconst_ops_thres = option.unconst_ops_thres;
  poros_option.poros_file = option.poros_file;
  poros_option.prewarm_datatypes = option.input_dtypes;
  poros_option.enable_fp16 = option.trt_enable_fp16;
  poros_option.max_batch_size = option.trt_max_batch_size;
  poros_option.max_workspace_size = option.trt_max_workspace_size;
  // poros_option.max_shape = option.trt_max_shape;
  // poros_option.min_shape = option.trt_min_shape;
  // poros_option.opt_shape = option.trt_opt_shape;
  FDASSERT(option.model_format == Frontend::TORCHSCRIPT,
           "PorosBackend only support model format of Frontend::TORCHSCRIPT.");
  backend_ = utils::make_unique<PorosBackend>();
  auto casted_backend = dynamic_cast<PorosBackend*>(backend_.get());
  FDASSERT(
      casted_backend->InitFromTorchscript(option.model_file, poros_option),
      "Load model from Torchscript failed while initliazing PorosBackend.");
#else
  FDASSERT(false,
           "PorosBackend is not available, please compiled with "
           "ENABLE_POROS_BACKEND=ON.");
#endif
}

}  // namespace fastdeploy
