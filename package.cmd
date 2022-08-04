pyinstaller stereo_main.py
python scripts/post_package.py
Xcopy \\10.0.0.3\ai\MaterialScanner\0.1.0\taichi\ .\dist\stereo_main\taichi\ /E /Y
copy .\func\texture_ao.py .\dist\stereo_main\func\texture_ao.py

pause