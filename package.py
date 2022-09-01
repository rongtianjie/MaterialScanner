import os
from scripts.post_package import check

os.system(r'pyinstaller main.py')

check()

os.system(r'Xcopy \\10.0.0.5\ai\MaterialScanner\0.1.0\taichi\ .\dist\main\taichi\ /E /Y')
os.system(r'copy .\func\texture_ao.py .\dist\main\func\texture_ao.py')
