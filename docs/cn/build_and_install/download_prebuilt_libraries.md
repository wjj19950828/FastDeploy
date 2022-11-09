# 预编译库安装

FastDeploy提供各平台预编译库，供开发者直接下载安装使用。当然FastDeploy编译也非常容易，开发者也可根据自身需求编译FastDeploy。

本文分为两部分：
- [1.GPU部署环境](##GPU部署环境)
- [2.CPU部署环境](##CPU部署环境)

## GPU部署环境

### 环境要求
- CUDA >= 11.2
- cuDNN >= 8.0
- python >= 3.6
- OS: Linux(x64)/Windows 10(x64)

支持CPU和Nvidia GPU的部署，默认集成Paddle Inference、ONNX Runtime、OpenVINO以及TensorRT推理后端，Vision视觉模型模块，Text文本NLP模型模块

### Python安装

Release版本（当前最新0.5.0）安装
```bash
pip install fastdeploy-gpu-python -f https://www.paddlepaddle.org.cn/whl/fastdeploy.html
```

Develop版本（Nightly build）安装
```bash
pip install fastdeploy-gpu-python==0.0.0 -f https://www.paddlepaddle.org.cn/whl/fastdeploy_nightly_build.html
```

其中推荐使用Conda配置开发环境
```bash
conda config --add channels conda-forge && conda install cudatoolkit=11.2 cudnn=8.2
```

### C++ SDK安装

Release版本（当前最新0.5.0）

| 平台 | 文件 | 说明 |
| :--- | :--- | :---- |
| Linux x64 | [fastdeploy-linux-x64-gpu-0.5.0.tgz](https://bj.bcebos.com/fastdeploy/release/cpp/fastdeploy-linux-x64-gpu-0.5.0.tgz) | g++ 8.2, CUDA 11.2, cuDNN 8.2编译产出 |
| Windows x64 | [fastdeploy-win-x64-gpu-0.5.0.zip](https://bj.bcebos.com/fastdeploy/release/cpp/fastdeploy-win-x64-gpu-0.5.0.zip) | Visual Studio 16 2019, CUDA 11.2, cuDNN 8.2编译产出 |

Develop版本（Nightly build）

| 平台 | 文件 | 说明 |
| :--- | :--- | :---- |
| Linux x64 | [fastdeploy-linux-x64-gpu-0.0.0.tgz](https://fastdeploy.bj.bcebos.com/dev/cpp/fastdeploy-linux-x64-gpu-0.0.0.tgz) | g++ 8.2, CUDA 11.2, cuDNN 8.2编译产出 |
| Windows x64 | [fastdeploy-win-x64-gpu-0.0.0.zip](https://fastdeploy.bj.bcebos.com/dev/cpp/fastdeploy-win-x64-gpu-0.0.0.zip) | Visual Studio 16 2019, CUDA 11.2, cuDNN 8.2编译产出 |

## CPU部署环境

### 环境要求
- python >= 3.6
- OS: Linux(x64/aarch64)/Windows 10 x64/Mac OSX(x86/aarm64)

仅支持CPU部署，默认集成Paddle Inference、ONNX Runtime、OpenVINO, Vision视觉模型模块(Linux aarch64和Mac OSX下仅集成ONNX Runtime模块)， Text文本NLP模型模块。

### Python安装

Release版本（当前最新0.5.0）安装
```bash
pip install fastdeploy-python -f https://www.paddlepaddle.org.cn/whl/fastdeploy.html
```

Develop版本（Nightly build）安装
```bash
pip install fastdeploy-python==0.0.0 -f https://www.paddlepaddle.org.cn/whl/fastdeploy_nightly_build.html
```

## C++ SDK安装

Release版本（当前最新0.5.0，Android为0.4.0 pre-release）

| 平台 | 文件 | 说明 |
| :--- | :--- | :---- |
| Linux x64 | [fastdeploy-linux-x64-0.5.0.tgz](https://bj.bcebos.com/fastdeploy/release/cpp/fastdeploy-linux-x64-0.5.0.tgz) | g++ 8.2编译产出 |
| Windows x64 | [fastdeploy-win-x64-0.5.0.zip](https://bj.bcebos.com/fastdeploy/release/cpp/fastdeploy-win-x64-0.5.0.zip) | Visual Studio 16 2019编译产出 |
| Mac OSX x64 | [fastdeploy-osx-x86_64-0.4.0.tgz](https://bj.bcebos.com/fastdeploy/release/cpp/fastdeploy-osx-x86_64-0.4.0.tgz) | clang++ 10.0.0编译产出|
| Mac OSX arm64 | [fastdeploy-osx-arm64-0.4.0.tgz](https://bj.bcebos.com/fastdeploy/release/cpp/fastdeploy-osx-arm64-0.4.0.tgz) | clang++ 13.0.0编译产出 |
| Linux aarch64 | [fastdeploy-linux-aarch64-0.4.0.tgz](https://bj.bcebos.com/fastdeploy/release/cpp/fastdeploy-linux-aarch64-0.4.0.tgz) | g++ 6.3.0编译产出 |  
| Android armv7&v8 | [fastdeploy-android-0.4.0-shared.tgz](https://bj.bcebos.com/fastdeploy/release/android/fastdeploy-android-0.4.0-shared.tgz) | NDK 25及clang++编译产出, 支持arm64-v8a及armeabi-v7a |

Develop版本（Nightly build）

| 平台 | 文件 | 说明 |
| :--- | :--- | :---- |
| Linux x64 | [fastdeploy-linux-x64-0.0.0.tgz](https://fastdeploy.bj.bcebos.com/dev/cpp/fastdeploy-linux-x64-0.0.0.tgz) | g++ 8.2编译产出 |
| Windows x64 | [fastdeploy-win-x64-0.0.0.zip](https://fastdeploy.bj.bcebos.com/dev/cpp/fastdeploy-win-x64-0.0.0.zip) | Visual Studio 16 2019编译产出 |
| Mac OSX x64 | - | - |
| Mac OSX arm64 | [fastdeploy-osx-arm64-0.0.0.tgz](https://fastdeploy.bj.bcebos.com/dev/cpp/fastdeploy-osx-arm64-0.0.0.tgz) | clang++ 13.0.0编译产出 |
| Linux aarch64 | - | - |  
| Android armv7&v8 | - | - |
