# cython:language_level=3
from loguru import logger
import time, functools
import sys, os
import json
import datetime
import rawpy
import numpy as np
from glob import glob
import multiprocessing as mp
from tqdm import tqdm, trange
from concurrent.futures import ProcessPoolExecutor, wait, as_completed

def set_log_level(console_level="INFO", file_level="DEBUG"):
    log_format = '<green>{time:YYYY-MM-DD HH:mm:ss}</green> | <level>{level: <8}</level> | <level>{message}</level> | <cyan>{name}</cyan>:<cyan>{function}</cyan>:<cyan>{line}</cyan>'
    if console_level in ["DEBUG", "INFO", "ERROR"] and file_level in ["DEBUG", "INFO", "ERROR"]:
        logger.remove()
        logger.add(sys.stdout,  format=log_format, level=console_level, enqueue=True)
        logger.debug(f"Set console log level to {console_level}.")
        logger.add("log/log_{time:YYYY_MM_DD}.log", format=log_format, level=file_level, enqueue=True)
        logger.debug(f"Set file log level to {file_level}.")

def count_time(fn):
    @functools.wraps(fn)
    def wrapper(*args,**kw):
        logger.info(f'+-+-+-+-+ Function {fn.__module__}::{fn.__name__}(..) started. +-+-+-+-+')
        t1=time.perf_counter()
        result = fn(*args,**kw)
        t2=time.perf_counter()
        logger.success(f'+-+-+-+-+ Function {fn.__module__}::{fn.__name__}(..) ended. Cost {t2-t1}s +-+-+-+-+')
        return result
    return wrapper

def exit_with_error(msg):
    logger.error(msg)
    raise Exception(msg)

def save_json(version, lens, scale, focus_id, path):
    now_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    dict = {"START_DATE": now_time, 
            "VERSION": version, 
            "LensModel": lens, 
            "Resolution": str(int(8/scale))+"K", 
            "Focus_ID": focus_id}
            
    with open(path, 'w') as f:
        json.dump(dict, f, indent=4)

def add2json(dict, path):
    with open(path, "r") as f:
        content = json.load(f)
    content.update(dict)
    with open(path, "w") as f:
        json.dump(content, f, indent=4)

def delete_cache(path_list):
    for f in path_list:
        if os.path.exists(f):
            try:
                os.remove(f)
            except:
                pass

class HideOutput(object):
    '''
    A context manager that block stdout for its scope, usage:

    with HideOutput():
        os.system('ls -l')
    '''

    def __init__(self, *args, **kw):
        sys.stdout.flush()
        self._origstdout = sys.stdout
        self._oldstdout_fno = os.dup(sys.stdout.fileno())
        self._devnull = os.open(os.devnull, os.O_WRONLY)

    def __enter__(self):
        self._newstdout = os.dup(1)
        os.dup2(self._devnull, 1)
        os.close(self._devnull)
        sys.stdout = os.fdopen(self._newstdout, 'w')

    def __exit__(self, exc_type, exc_val, exc_tb):
        sys.stdout = self._origstdout
        sys.stdout.flush()
        os.dup2(self._oldstdout_fno, 1)