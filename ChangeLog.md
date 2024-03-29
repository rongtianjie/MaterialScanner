# 材质扫描仪双目2.0旋转光源算法更新日志
***

## v0.9.1 - 2022.10.14
- 照片检测、重建照片 IO 及光场计算使用多进程加速

## v0.9.0 - 2022.10.13
- 适配 50 mm 镜头材质扫描仪新版多目重建方式
- 优化数据格式降低读写照片时的显存占用，提升速度

## v0.8.1 - 2022.09.29
- 配置文件控制去马赛克方式
- 根据显存大小自动调整并行线程数

## v0.8.0 - 2022.09.28
- 修复读入照片颜色通道顺序命名错误
- 相机内外参从输入文件夹读入
- GPU 运行 MHC demosaic 模块完成开发，提升照片 IO 速度，测试通过 (PSNR ≈ 30 dB)

| | GPU (24G显存) | CPU (64G 内存) |
| ----------- | ----------- | ----------- |
| 并行线程数 | 3 | 8 |
| 处理48张RAW照片时间 | ≈ 4 min | ≈ 8 min |

> 24G显存GPU上，最大并行线程数为 3

## v0.7.1 - 2022.09.23
- 配置文件独立相机内外参

## v0.7.0 - 2022.09.22
- 根据系统性能动态图片 IO 多线程数(可通过配置文件控制)
- 优化照片 IO 流程，节省一次照片 IO 时间
- 法线矫正模块 GPU 调用逻辑更新

## v0.6.2 - 2022.09.21
- 增加前置照片检查模块
- 增加 Log 信息，优化 Log 文件记录方式
- 优化线程数控制