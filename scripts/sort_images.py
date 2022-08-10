# This script sorts images for rotate light system (light_num = 16).

from tqdm import trange, tqdm
import shutil
from glob import glob
import os

path = r'Z:\双目\20220808旋转光源拍摄瓷砖\cizhuanxiao'
start = 0
rot = 8

for cam in ['mid', 'side']:
    fl = sorted(glob(os.path.join(path, 'ori', cam, '*.RAF')))[start : start+rot*16]
    grid = [[i,j] for i in range(rot) for j in range(16)]
    for i, j in tqdm(grid):
        idx_list = [rot+i+1, rot*3+i+1, rot*5+i+1, rot*7+i+1, i+1, rot*2+i+1, rot*4+i+1, rot*6+i+1]
        idx_list = idx_list + [x+rot*8 for x in idx_list]
    
        if idx_list[j] < 10:
            idx_list[j] = '00' + str(idx_list[j])
        elif idx_list[j] < 100:
            idx_list[j] = '0' + str(idx_list[j])
        else:
            idx_list[j] = str(idx_list[j])

        f_in = fl[i*16+j]
        f_out = os.path.join(path, cam, 'MS'+idx_list[j]+'.RAF')
        shutil.copy(f_in, f_out)  