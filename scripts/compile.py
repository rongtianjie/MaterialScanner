from setuptools import setup
from Cython.Build import cythonize
import os 
import shutil
from glob import glob
import sys

# Compile command:
# python .\scripts\compile.py build_ext --inplace

dist_path = "dist/main_compile"
COPY_ENV = True
COPY_CONFIG = True

def get_ext_modules():
    ext_modules = []
    for dir in ["base", "func", "process"]:
        os.makedirs(os.path.join(dist_path, dir), exist_ok=True)
        ext_modules += glob(os.path.join(os.getcwd(), dir, "*.py"))

    # remove some of the files
    ext_modules.remove(os.path.join(os.getcwd(), "func", "texture_ao.py"))
    ext_modules.remove(os.path.join(os.getcwd(), "func", "addon_segmentation.py"))

    return ext_modules

try:
    shutil.rmtree(dist_path)
except:
    pass
os.makedirs(dist_path)

setup(
    name='compile',
    ext_modules=cythonize(get_ext_modules()),
)

for dir in ["base", "func", "process"]:
    for f in glob(os.path.join(os.getcwd(), dir, "*.c")):
        file_head = os.path.basename(f).split(".")[0]
        file = glob(os.path.join(os.getcwd(), file_head + "*.pyd"))[0]
        print(f"Moving {file}")
        shutil.move(file, os.path.join(os.getcwd(), dist_path, dir))

        print(f"Removing {f}")
        os.remove(f)

# copy texture_ao.py
shutil.copy(os.path.join(os.getcwd(), "func", "texture_ao.py"), os.path.join(os.getcwd(), dist_path, "func"))
# copy addon_segmentation.py
shutil.copy(os.path.join(os.getcwd(), "func", "addon_segmentation.py"), os.path.join(os.getcwd(), dist_path, "func"))

shutil.copy("main.py", os.path.join(os.getcwd(), dist_path))
shutil.copy("example.py", os.path.join(os.getcwd(), dist_path))
shutil.copy("./scripts/run.cmd", os.path.join(os.getcwd(), dist_path))

if COPY_CONFIG:
    shutil.copytree("conf", os.path.join(dist_path, "conf"))

if COPY_ENV:
    print("Copying env...")
    paths = sys.path
    for path in paths:
        if path.endswith("ma_package"):
            shutil.copytree(path, os.path.join(dist_path, "Python39"))
            print("Python environment copied.")
            break

print("Done.")