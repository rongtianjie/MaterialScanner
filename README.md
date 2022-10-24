# MaterialScanner
新版材质扫描仪算法程序

### Change Log
***
[点击跳转](ChangeLog.md)
 
### Python环境
***

见 `requirements.txt`， 由 `pipreqs` 生成
- 装环境
```
pip install -r requirements.txt
```
- 导出pip环境到当前目录下的 `requirements.txt`

1. 安装pipreqs
```
pip install pipreqs
```
2. 导出 requirements.txt
```
pipreqs . --encoding=utf8 --force
```

#### Exiftool
1. 下载 https://exiftool.org/exiftool-12.40.zip
2. 解压缩并且把 `.exe` 文件名改成 `exiftool.exe`
3. ~~把这个 `.exe` 放到 `scripts` 下面，比如如果没有用 conda，那么就是 `C:\Users\...\AppData\Local\Programs\Python\Python39\Scripts`~~
> **[中文支持]** 在高版本的 python（好像 3.6 不需要但是 3.9 需要的，应该是这之间 python 更新的 encoding 逻辑导致的），如果要支持中文路径，需要对 python 接口的源码进行hack。如果你运行有空格或者中文路径的数据，在 `exiftool` 里面报了和 encoding 相关的错误，可以尝试找到你的 python 安装环境下 `pyexiftool` 模块，把 `exiftool.py` 923 行:
```
raw_stdout = _read_fd_endswith(fdout, seq_ready.encode(self._encoding), self._block_size).decode(self._encoding)
```
改成:
```
raw_before_decoding = _read_fd_endswith(fdout, seq_ready.encode(self._encoding), self._block_size)
try:
    raw_stdout = raw_before_decoding.decode(self._encoding)
except Exception as e:
    raw_stdout = raw_before_decoding.decode('utf-8')
```

### 运行
***
- 直接运行
双击 `run.cmd` ，运行前将会检查运行环境，如果缺失，则会从网盘（10.0.0.3，需要电脑能访问）上自动拷贝依赖文件（exe、dll、预置数据等），运行的是 `example.py` 文件。

- 命令行
`main.py` 文件为命令行封装，参数如下：
    + -i 输入文件夹路径
    + -o 输出文件夹路径
    + -l 镜头参数，选项为{30， 50， 120}
    + -s 缩放系数，选项为{1，2，4，8}， 默认为1
    + -c 使用缓存

### 输出
***
```
-- folder_name
    | out
        | acmmp_reference
        | reference
        | AlgorithmPara.json
        | T_{folder_name}_Albedo_{scale}K.png
        | T_{folder_name}_Albedo_Corrected_{scale}K.png
        | T_{folder_name}_Normal_{scale}K.png
        | T_{folder_name}_Normal_Original_{scale}K.png
        | T_{folder_name}_Normal_Plain_{scale}K.png
        | T_{folder_name}_Normal_Shadow_Original_{scale}K.png
        | T_{folder_name}_Roughness_0_{scale}K.png
        | T_{folder_name}_Roughness_2_{scale}K.png
        | T_{folder_name}_Roughness_ABD_{scale}K.png
        | T_{folder_name}_AO_{scale}K.png
        | T_{folder_name}_Displacement_{scale}K.exr
```

### 打包
***

```
python .\scripts\compile.py build_ext --inplace
```

### 代码结构简介
***

* base
    + common.py
        - 日志相关，使用`loguru`库实现
        - 计时程序，使用python装饰器，在需要计时的方法定义上加  `@count_time` 即可
    + common_isp.py `读取raw格式的接口`
    + common_image.py `封装读取图片文件夹的方法`
    + config_handler.py `配置文件读取`

* conf `配置文件目录`
* func `主要算法目录`
    + texture_xxx.py `对应种类贴图的算法`
    + calib_xxx.py `保留的标定方法` 
* process `流程调度文件目录`
    + stereo.py `当前的双目解算流程`
    + mono.py `当前的单目解算流程`
* scripts `工具脚本目录`
* main.py `命令行封装`
* example.py `本地测试代码`