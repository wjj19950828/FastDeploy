
# How to Build GPU Deployment Environment

FastDeploy supports Paddle Inference, ONNX Runtime and TensorRT in the GPU environment. It should be noted that users also can use CPU in a GPU environment in Linux & Windows, so users can integrate the OpenVINO backend at the same time.

| Backend               | Platform                                             | Supported model format | Description                                                                                 |
|:--------------------- |:---------------------------------------------------- |:---------------------- |:------------------------------------------------------------------------------------------- |
| Paddle&nbsp;Inference | Windows(x64)<br>Linux(x64)                           | Paddle                 | Support both CPU/GPU, and compilation switch is `ENABLE_PADDLE_BACKEND`. The default is OFF |
| ONNX&nbsp;Runtime     | Windows(x64)<br>Linux(x64/aarch64)<br>Mac(x86/arm64) | Paddle/ONNX            | Support both CPU/GPU, and compilation switch is `ENABLE_ORT_BACKEND`. The default is OFF    |
| TensorRT              | Windows(x64)<br>Linux(x64)                           | Paddle/ONNX            | Support GPU only, and compilation switch is `ENABLE_TRT_BACKEND`. The default is OFF        |
| OpenVINO              | Windows(x64)<br>Linux(x64)                           | Paddle/ONNX            | Support CPU only, and compilation switch is `ENABLE_OPENVINO_BACKEND`. The default is OFF   |

Note:

When the environment is GPU, please set `WITH_GPU` as ON and specify `CUDA_DIRECTORY`. If TensorRT integration is needed, please specify `TRT_DIRECTORY` as well.

## How to Build and Install C++ SDK

### Linux

Prerequisite for Compiling on Linux:

- gcc/g++ >= 5.4 (8.2 is recommended)
- cmake >= 3.18.0
- cuda >= 11.2
- cudnn >= 8.2

```
git clone https://github.com/PaddlePaddle/FastDeploy.git
cd FastDeploy
mkdir build && cd build
cmake .. -DENABLE_ORT_BACKEND=ON \
         -DENABLE_PADDLE_BACKEND=ON \
         -DENABLE_OPENVINO_BACKEND=ON \
         -DENABLE_TRT_BACKEND=ON \
         -DWITH_GPU=ON \
         -DTRT_DIRECTORY=/Paddle/TensorRT-8.4.1.5 \
         -DCUDA_DIRECTORY=/usr/local/cuda \
         -DCMAKE_INSTALL_PREFIX=${PWD}/compiled_fastdeploy_sdk \
         -DENABLE_VISION=ON
make -j12
make install
```

### Windows

Prerequisite for Compiling on Windows:

- Windows 10/11 x64
- Visual Studio 2019
- cuda >= 11.2
- cudnn >= 8.2

Notice: Make sure `Visual Studio Integration` is installed during CUDA installation, or manually copy the 4 files under `C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.2\extras\visual_studio_integration\MSBuildExtensions\` into `C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Microsoft\VC\v160\BuildCustomizations\`. Otherwise, you may run into `No CUDA toolset found` error during cmake.

Launch the x64 Native Tools Command Prompt for VS 2019 from the Windows Start Menu and run the following commands:

```
git clone https://github.com/PaddlePaddle/FastDeploy.git
cd FastDeploy
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019" -A x64 \
         -DENABLE_ORT_BACKEND=ON \
         -DENABLE_PADDLE_BACKEND=ON \
         -DENABLE_OPENVINO_BACKEND=ON \
         -DENABLE_TRT_BACKEND=ON
         -DENABLE_VISION=ON \
         -DWITH_GPU=ON \
         -DTRT_DIRECTORY="D:\Paddle\TensorRT-8.4.1.5" \
         -DCUDA_DIRECTORY="C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.2" \
         -DCMAKE_INSTALL_PREFIX="D:\Paddle\compiled_fastdeploy"
msbuild fastdeploy.sln /m /p:Configuration=Release /p:Platform=x64
msbuild INSTALL.vcxproj /m /p:Configuration=Release /p:Platform=x64
```

Once compiled, the C++ inference library is generated in the directory specified by `CMAKE_INSTALL_PREFIX`

If you use CMake GUI, please refer to [How to Compile with CMakeGUI + Visual Studio 2019 IDE on Windows](../faq/build_on_win_with_gui.md)

## How to Build and Install Python SDK

### Linux

Prerequisite for Compiling on Linux:

- gcc/g++ >= 5.4 (8.2 is recommended)

- cmake >= 3.18.0

- python >= 3.6

- cuda >= 11.2

- cudnn >= 8.2

All compilation options are imported via environment variables

```
git clone https://github.com/PaddlePaddle/FastDeploy.git
cd FastDeploy/python
export ENABLE_ORT_BACKEND=ON
export ENABLE_PADDLE_BACKEND=ON
export ENABLE_OPENVINO_BACKEND=ON
export ENABLE_VISION=ON
export ENABLE_TRT_BACKEND=ON
export WITH_GPU=ON
export TRT_DIRECTORY=/Paddle/TensorRT-8.4.1.5
export CUDA_DIRECTORY=/usr/local/cuda

python setup.py build
python setup.py bdist_wheel
```

### Windows

Prerequisite for Compiling on Windows:

- Windows 10/11 x64
- Visual Studio 2019
- python >= 3.6
- cuda >= 11.2
- cudnn >= 8.2

Launch the x64 Native Tools Command Prompt for VS 2019 from the Windows Start Menu and run the following commands:

```
git clone https://github.com/PaddlePaddle/FastDeploy.git
cd FastDeploy/python
set ENABLE_ORT_BACKEND=ON
set ENABLE_PADDLE_BACKEND=ON
set ENABLE_OPENVINO_BACKEND=ON
set ENABLE_VISION=ON
set ENABLE_TRT_BACKEND=ON
set WITH_GPU=ON
set TRT_DIRECTORY=D:\Paddle\TensorRT-8.4.1.5
set CUDA_DIRECTORY=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.2

python setup.py build
python setup.py bdist_wheel
```

The compiled `wheel` package will be generated in the `FastDeploy/python/dist` directory once finished. Users can pip-install it directly.

During the compilation, if developers want to change the compilation parameters, it is advisable to delete the `build` and `.setuptools-cmake-build` subdirectories in the `FastDeploy/python` to avoid the possible impact from cache, and then recompile.
