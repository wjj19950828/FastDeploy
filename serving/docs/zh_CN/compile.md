# 服务化部署镜像编译

本文档介绍如何制作FastDploy镜像

## 制作GPU镜像

FastDploy发布的GPU镜像基于[Triton Inference Server](https://github.com/triton-inference-server/server)的21.10版本进行制作，如果有其他CUDA版本需求，可以参照[NVIDIA 官网](https://docs.nvidia.com/deeplearning/frameworks/support-matrix/index.html)中展示的版本信息修改Dockerfile和scripts中的脚本.

```
# 进入serving目录执行脚本编译fastdeploy和服务化的backend
cd serving
bash scripts/build.sh

# 退出到FastDeploy主目录，制作镜像
cd ../
docker build -t paddlepaddle/fastdeploy:0.3.0-gpu-cuda11.4-trt8.4-21.10 -f serving/Dockerfile .
```

## 制作CPU镜像

```
# 进入serving目录执行脚本编译fastdeploy和服务化的backend
cd serving
bash scripts/build.sh OFF

# 退出到FastDeploy主目录，制作镜像
cd ../
docker build -t paddlepaddle/fastdeploy:0.3.0-cpu-only-21.10 -f serving/Dockerfile_cpu .
```

## 制作IPU镜像

```
# 进入serving目录执行脚本编译fastdeploy和服务化的backend
cd serving
bash scripts/build_fd_ipu.sh

# 退出到FastDeploy主目录，制作镜像
cd ../
docker build -t paddlepaddle/fastdeploy:0.3.0-ipu-only-21.10 -f serving/Dockerfile_ipu .
```
