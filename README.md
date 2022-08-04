# MaterialScanner
新版材质扫描仪算法程序
 
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


### 运行
***
- 直接运行
双击 `run.cmd` ，运行前将会检查运行环境，如果缺失，则会从网盘（10.0.0.3，需要电脑能访问）上自动拷贝依赖文件（exe、dll、预置数据等），运行的是 `example.py` 文件。

- 命令行
`stereo_main.py` 文件为命令行封装，参数如下：
    + -i 输入文件夹路径
    + -o 输出文件夹路径
    + -l 镜头参数，选项为{30， 50， 120}
    + -s 缩放系数，选项为{1，2，3，4}， 默认为1
    + -c 使用缓存

### 打包
***

双击 `package.cmd` ，将使用 `pyinstaller` 打包 `stereo_main.py` 文件为exe程序，供前台界面调用。

### 代码结构简介
***

* base
    + common.py
        - 日志相关，使用`loguru`库实现
        - 计时程序，使用python装饰器，在需要计时的方法定义上加  `@count_time` 即可
    + common_isp.py `读取raw格式的接口`
    + common_image.py `封装读取图片文件夹的方法`

* conf `配置文件目录`
* func `主要算法目录`
    + texture_xxx.py `对应种类贴图的算法`
    + calib_xxx.py `保留的标定方法` 
* process `流程调度文件目录`
    + stereo.py `当前的双目解算流程`
* scripts `工具脚本目录`
* stereo_main.py `命令行封装`
* example.py `本地测试代码`
* run.cmd `运行本地测试代码命令，加入了运行前依赖检查`
* package.cmd `打包命令`