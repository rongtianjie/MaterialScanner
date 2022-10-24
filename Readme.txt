一，编译
mkdir build
cd build
cmake ..
make

二，运行
创建test文件夹
	test/input/mid/0.RAF
	test/input/right/0.RAF
修改config/120_stereo.json文件
运行时将 生成的easy_calib_dev.exe 与 dcraw_emu.exe libraw.dll  120_stereo.json 放在同一目录下