# from curses import mousemask
from dataclasses import replace
from re import S
from unittest import makeSuite
import numpy as np
from scipy import sparse as sp
from scipy.sparse.linalg import spsolve, inv, cg
import cv2
import os
import json
import scipy.io as sio
import sys
import time
import numpy.ma as ma

class Semi_calib(object):
    def __init__(self,config_path):
        with open(config_path) as json_file:
            config=json.load(json_file)
        self.data = config["data"]

        self.params=config["params"]

        self.estimator = self.params["estimator"]

        if "lambd" not in self.params.keys():
            self.lamda=0
        elif self.params["lambd"]<0:
            raise ValueError("lambd must >0")
        else:
            self.lamda=self.params["lambd"]

        if "zeta" not in self.params.keys():
            self.zeta=0
        elif self.params["zeta"]<0:
            raise ValueError("zeta must >0")
        else:
            self.zeta=self.params["zeta"]

        if "self_shadows" not in self.params.keys():
            self.self_shadows=1
        elif self.params["self_shadows"] in [0,1]:
            self.self_shadows=self.params["self_shadows"]
        else:
            raise ValueError("self_shadows must be binary")

        if "scales" not in self.params.keys():
            self.scales=1
        elif self.params["scales"]<=0:
            raise ValueError("scales must >0")
        else:
            self.scales=self.params["scales"]

        if "maxit" not in self.params.keys():
            self.maxit=100
        elif self.params["maxit"]<0:
            raise ValueError("maxit must >0")
        else:
            self.maxit=self.params["maxit"]

        if "tol" not in self.params.keys():
            self.tol=1e-3
        elif self.params["tol"]<0:
            raise ValueError("tol must >0")
        else:
            self.tol=self.params["tol"]

        if "tol_normals" not in self.params.keys():
            self.tol_normals=0.1
        elif self.params["tol_normals"]<0:
            raise ValueError("tol_normals must >0")
        else:
            self.tol_normals=self.params["tol_normals"]

        if "precond" not in self.params.keys():
            self.precond="ichol"
        elif self.params["precond"] in ['cmg','ichol','jacobi','gs']:
            self.precond=self.params["precond"]
        else:
            raise ValueError("unknown preconditioner")

        if "display" not in self.params.keys():
            self.display=0
        elif self.params["display"] in [0,1]:
            self.self_shadows=self.params["display"]
        else:
            raise ValueError("display must be binary")

        if "tol_pcg" not in self.params.keys():
            self.tol_pcg=1e-6
        elif self.params["tol_pcg"]<0:
            raise ValueError("ratio must >0")
        else:
            self.tol_pcg=self.params["tol_pcg"]

        if "maxit_pcg" not in self.params.keys():
            self.maxit_pcg=25
        elif self.params["maxit_pcg"]<0:
            raise ValueError("maxit_pcg must >0")
        else:
            self.maxit_pcg=self.params["maxit_pcg"]
        

        self.z0 = self.data['z0']

        self.obj_name = self.params['obj_name']
        self.use_depth = self.params['use_depth']
        self.cal_shadow = self.params['cal_shadow']
        self.size = self.params['size']
        self.scales = self.params['scales']
        self.zeta = self.params['zeta']
        self.save_current_result = self.params['save_current_result']
        self.cos4a = self.params['cos4a']






    def load_imgs(self, imgs_dir_list, size=1, K = None, cos4a=True):
        imgs = np.array([cv2.imread(x, -1)[::size,::size,:] for x in imgs_dir_list])
        if cos4a and K is not None:
            ff = 0.5*(K[0,0]+K[1,1])
            xx,yy = np.meshgrid(np.arange(imgs.shape[2]), np.arange(imgs.shape[1]))
            xx = xx-K[0,2]; yy = yy-K[1,2]
            cos4a = np.power(ff/np.sqrt(xx**2+yy**2+ff**2), 4)

            return imgs/cos4a[None, :, :, None]
        else:
            return imgs

    def get_t_fcn(self, z, Xs, Dir, mu, u_tilde, v_tilde):
        '''
        input:
            z: [n_px,1]
            Xs: [n_img,3]
            Dir: [n_img,3]
            u_tilde/v_tilde:[n_px,1]
        outputs:
        '''
        npix = len(z)
        nimgs = Xs.shape[0]

        # Current mesh
        exp_z = np.exp(z)
        XYZ = np.concatenate((exp_z * u_tilde, exp_z * v_tilde, exp_z), axis=1)

        # T field
        T_field = np.zeros((3, npix, nimgs))
        a_field = np.zeros((npix, nimgs))
        da_field = np.zeros((npix, nimgs))

        for i in range(nimgs):
            T_field[0, :, i] = Xs[i, 0] - XYZ[:, 0]
            T_field[1, :, i] = Xs[i, 1] - XYZ[:, 1]
            T_field[2, :, i] = Xs[i, 2] - XYZ[:, 2]
            normS_i=np.sqrt(T_field[0,:,i]**2+T_field[1,:,i]**2+T_field[2,:,i]**2)
            
            # Attenuation = anisotropy / squared distance
            scal_prod=T_field[0,:,i]*Dir[i,0]+T_field[1,:,i]*Dir[i,1]+T_field[2,:,i]*Dir[i,2]
            a_field[:,i]=(scal_prod**mu)/(normS_i**(3+mu))
            da_field[:, i] = (mu * scal_prod ** (mu - 1) * (XYZ[:, 0] * Dir[i, 0] + XYZ[:, 1] * Dir[i, 1] + XYZ[:, 2] * Dir[i, 2])) / (normS_i ** (mu + 3)) - ((mu + 3) * (scal_prod ** mu) * (-T_field[0, :, i] * XYZ[:, 0] - T_field[1, :, i] * XYZ[:, 1] - T_field[2, :,i] * XYZ[:,2]) / (normS_i ** (mu + 5)))

            # Final lighting field
            T_field[0,:,i] *= a_field[:,i]
            T_field[1,:,i] *= a_field[:, i]
            T_field[2,:,i] *= a_field[:, i]

        grad_t=np.zeros((3,npix,nimgs))
        grad_t[0] = np.repeat(-exp_z*u_tilde,a_field.shape[1],axis=1)*(a_field+da_field)+(da_field*Xs[:,0].T)
        grad_t[1] = np.repeat(-exp_z * v_tilde, a_field.shape[1], axis=1) * (a_field + da_field) + (da_field * Xs[:, 1].T)
        grad_t[2] = np.repeat(-exp_z , a_field.shape[1], axis=1) * (a_field + da_field) + (da_field * Xs[:, 2].T)

        return [T_field.transpose(2,1,0), grad_t.transpose(2,1,0)]

    def get_phi_fcn(self, x, lamda, estimator='Cauchy'):
        '''
            estimator: ['LS', 'Cauchy', 'LP', 'GM', 'Welsh']
        '''
        if estimator == 'LS':
            phi_fcn = 0.5*x**2
        elif estimator == 'Cauchy':
            phi_fcn = 0.5*np.log(1+(x**2)/(lamda**2))
        elif estimator == 'Lp':
            thr_norm = 1e-2
            phi_fcn = np.power(np.abs(x), lamda)/(lamda*(np.power(thr_norm, lamda-2)))
        elif estimator == 'GM':
            phi_fcn = 0.5*lamda**2*x**2/(x**2+lamda**2)
        elif estimator == 'Welsh':
            phi_fcn = 0.5*lamda**2*(1-np.exp(-x**2/(lamda**2)))
        else:
            raise '''Estimator 需在['LS', 'Cauchy', 'LP', 'GM', 'Welsh']中！！！'''        
        return phi_fcn

    def get_w_fcn(self, x, lamda, estimator='Cauchy'):
        '''
            estimator: ['LS', 'Cauchy', 'LP', 'GM', 'Welsh']
        '''

        if estimator == 'LS':
            w_fcn = np.ones_like(x)
        elif estimator == 'Cauchy':
            w_fcn = 1/(lamda**2 + x**2)
        elif estimator == 'Lp':
            thr_norm = 1e-2
            x_ = np.abs(x.copy()); x_[thr_norm>x_]=thr_norm
            w_fcn = lamda*(np.power(x_, lamda-2))/(lamda*(np.power(thr_norm, lamda-2)))
        elif estimator == 'GM':
            w_fcn = np.power(lamda, 4)/((x**2+lamda**2)**2)
        elif estimator == 'Welsh':
            w_fcn = np.exp(-x**2/(lamda**2))
        else:
            raise '''Estimator 需在['LS', 'Cauchy', 'LP', 'GM', 'Welsh']中！！！'''
        
        return w_fcn

    def get_chi_fcn(self, x, self_shadows):
        if self_shadows:
            chi_fcn = (x>0).astype(np.float32)
        else:
            chi_fcn = np.ones_like(x).astype(np.float32)
        return chi_fcn

    def get_psi_fcn(self, x, self_shadows):

        if self_shadows:
            psi_fcn = np.max(x, 0)
        else:
            psi_fcn = x
        return psi_fcn

    def get_Q(self, fx,fy,u,v):
        Q = np.zeros((self.n_px, 3, 3))
        Q[:,0,0] = fx; Q[:,1,1] = fy
        Q[:,0,2] = -u; Q[:,1,2] = -v; Q[:,2,2] = 1
        return Q

    def get_shading_fcn(self, Tz, z_tilde, Dxy, Q):
        '''
        input: 
            Tz: [n_img, n_px, 3]
            z_tilde:[n_px, 1]
            Dxy: [2*n_px, n_px]
        output:
            shadz: [n_img, n_px]
        '''
        z_grident = (Dxy@z_tilde).reshape(self.n_px, 2)
        z_norm = np.hstack((z_grident, -1*np.ones((self.n_px, 1))))
        return np.sum(((Q@Tz.reshape(self.n_img, self.n_px, 3, 1)).reshape(self.n_img, self.n_px, 3)*z_norm), axis=-1)  # [n_img, n_px] 不改

    def get_r_fcn(self, rho, phi, shading, II):
        '''
        input:
            rho: [n_px, n_ch]
            phi: [n_img, n_ch]
            shading: [n_img, n_px]
            II: [n_img, n_px, n_ch]
        
        output:
            residual_fcn: [n_img, n_x, n_ch]
        '''
        return rho[None,:,:]*phi[:,None,:]*self.get_psi_fcn(shading, self.self_shadows)[:,:,None] - II

    def get_J_fcn(self, rho, phi, shading, II, lamda):
        '''
        input:
            rho: [n_px, n_ch]
            phi: [n_img, n_ch]
            shading: [n_img, n_px]
            II: [n_img, n_px, n_ch]
        output:
            J_fcn: energy
        '''
        return np.sum(self.get_phi_fcn(self.get_r_fcn( rho, phi, shading, II), lamda, estimator=self.estimator))

    def rowfind(self,arr, cond):
        '''
        arr_matrix 排序按照row排序
        :param arr:
        :param cond:
        :return:
        '''
        h, w = arr.shape
        arr_matrix = np.where(arr == cond)
        arr_matrix = np.array(arr_matrix)
        arr_res = np.array([w * arr_matrix[0][i] + arr_matrix[1][i] for i in range(0, len(arr_matrix[0]))])
        return arr_matrix, arr_res

    def gradient_operators_row(self,mask):
        '''

        :param mask: [H, W]
        :return:
        '''
        h, w = mask.shape
        Omega_padded = np.pad(mask, ((1, 1), (1, 1)), 'constant', constant_values=(0, 0))
        Omega = np.zeros((h, w, 4))
        # Pixels who have bottom neighbor in mask
        Omega[:, :, 0] = mask * Omega_padded[2:, 1:-1]
        # Pixels who have top neighbor in mask
        Omega[:, :, 1] = mask * Omega_padded[0:-2, 1:-1]
        # Pixels who have right neighbor in mask
        Omega[:, :, 2] = mask * Omega_padded[1:-1, 2:]
        # Pixels who have left neighbor in mask
        Omega[:, :, 3] = mask * Omega_padded[1:-1, 0:-2]

        imask_matrix, imask =  self.rowfind(mask, 1)

        index_matrix = np.zeros_like(mask)
        for i in range(0, len(imask_matrix[0])):
            index_matrix[imask_matrix[0][i], imask_matrix[1][i]] = i
        # print(index_matrix)
        [xc, yc] = np.where(Omega[:, :, 2] == 1)
        # indices_centre and indices_right pair
        indices_centre = index_matrix[xc, yc]
        indices_right = index_matrix[xc, yc + 1]
        II = indices_centre
        JJ = indices_right
        KK = np.ones_like(indices_centre)
        II = np.append(II, indices_centre)
        JJ = np.append(JJ, indices_centre)
        KK = np.append(KK, -np.ones_like(indices_centre))
        Dvp = sp.csr_matrix((KK, (II, JJ)), shape=(len(imask), len(imask)))

        [xc, yc] = np.where(Omega[:, :, 3] == 1)
        # indices_centre and indices_left pair
        indices_centre = index_matrix[xc, yc]
        indices_right = index_matrix[xc, yc - 1]

        II = indices_centre
        JJ = indices_right
        KK = -np.ones_like(indices_centre)
        II = np.append(II, indices_centre)
        JJ = np.append(JJ, indices_centre)
        KK = np.append(KK, np.ones_like(indices_centre))
        Dvm = sp.csr_matrix((KK, (II, JJ)), shape=(len(imask), len(imask)))

        [xc, yc] = np.where(Omega[:, :, 0] == 1)
        # indices_centre and indices_bottom pair
        indices_centre = index_matrix[xc, yc]
        indices_right = index_matrix[xc + 1, yc]

        II = indices_centre
        JJ = indices_right
        KK = np.ones_like(indices_centre)
        II = np.append(II, indices_centre)
        JJ = np.append(JJ, indices_centre)
        KK = np.append(KK, -np.ones_like(indices_centre))
        Dup = sp.csr_matrix((KK, (II, JJ)), shape=(len(imask), len(imask)))

        [xc, yc] = np.where(Omega[:, :, 1] == 1)
        # indices_centre and indices_top pair
        indices_centre = index_matrix[xc, yc]
        indices_right = index_matrix[xc - 1, yc]

        II = indices_centre
        JJ = indices_right
        KK = -np.ones_like(indices_centre)
        II = np.append(II, indices_centre)
        JJ = np.append(JJ, indices_centre)
        KK = np.append(KK, np.ones_like(indices_centre))
        Dum = sp.csr_matrix((KK, (II, JJ)), shape=(len(imask), len(imask)))

        return Dup, Dum, Dvp, Dvm, Omega, index_matrix, imask[:, np.newaxis]

    # def make_gradient(self,mask):
        '''

        :param mask: [H, W] with values of 0 and 1
        :return: Gradient Map sparse [m*2, m] in which m is the # of 1s in mask
        '''
        Dyp, Dym, Dxp, Dxm, Omega, index_matrix, imask = self.gradient_operators_row(mask)

        # When there is no bottom neighbor, replace by backward (or by 0 if no top)
        Dy = Dyp.copy()
        [xc, yc], no_bottom = self.rowfind(1 - Omega[:, :, 0], 1)
        no_bottom = (index_matrix[xc, yc][np.where(index_matrix[xc, yc] != 0)]).astype(int)
        Dy[no_bottom,:] = Dym[no_bottom,:].copy()
        Dx = Dxp.copy()
        [xc, yc], no_right =  self.rowfind(1 - Omega[:, :, 2], 1)
        no_right = (index_matrix[xc, yc][np.where(index_matrix[xc, yc] != 0)]).astype(int)
        Dx[no_right,:] = Dxm[no_right,:].copy()
        D = sp.hstack([Dx, Dy]).reshape(-1, len(imask))

        return D, Dx, Dy

    def make_gradient(self,mask):
            '''
            :param mask: [H, W] with values of 0 and 1
            :return: Gradient Map sparse [m*2, m] in which m is the # of 1s in mask
            '''
            Dyp, Dym, Dxp, Dxm, Omega, index_matrix, imask = self.gradient_operators_row(mask)

            # When there is no bottom neighbor, replace by backward (or by 0 if no top)
            Dy = Dyp
            [xc, yc], no_bottom = self.rowfind(1 - Omega[:, :, 0], 1)
            no_bottom = (index_matrix[xc, yc][np.where(index_matrix[xc, yc] != 0)]).astype(np.int64)
            print(Dy.shape,Dym.shape, no_bottom.shape)
            # Dy[no_bottom,:] = Dym[no_bottom,:]
            #########################################
            nob_list = np.array_split(no_bottom, 20)
            for nb in nob_list:
                Dy[nb,:] = Dym[nb,:]
            #########################################

            Dx = Dxp
            [xc, yc], no_right =  self.rowfind(1 - Omega[:, :, 2], 1)
            no_right = (index_matrix[xc, yc][np.where(index_matrix[xc, yc] != 0)]).astype(np.int64)

            # Dx[no_right,:] = Dxm[no_right,:]
            #########################################
            nor_list = np.array_split(no_right, 20)
            for nr in nor_list:
                Dx[nr,:] = Dxm[nr,:]
            #########################################
            D = sp.hstack([Dx, Dy]).reshape(-1, len(imask))

            return D, Dx, Dy


    def create_Dx_Dy(self, img_size):
        h,w = img_size
        row_u = []; col_u = []; value_u = []
        for i in range(h*w):
            row_u += [i, i]
            if i % w != w-1: 
                col_u += [i, i+1]
                value_u += [-1, 1]
            else:
                col_u += [i, i-1]
                value_u += [1, -1]

        row_v = []; col_v = []; value_v = []
        for i in range(h*w):
            row_v += [i, i]
            if i // w != h-1:
                col_v += [i, i+w]
                value_v += [-1, 1]
            else:
                col_v += [i, i-w]
                value_v += [1, -1]

        Du = sp.csr_matrix((value_u,(row_u, col_u)))
        Dv = sp.csr_matrix((value_v,(row_v, col_v)))
        D = sp.hstack([Du,Dv]).reshape(-1,h*w)
        return D, Du, Dv

    def pseudo_albedo_update(self,I,Phi,psi,chi,rho_tilde,lamda):
        '''
        input:
            I: [n_imgs,n_pix,n_ch]
            Phi: [n_imgs,n_ch]
            psi: [n_imgs,n_pix]
            chi:[n_imgs,n_pix]
            rho_tilde:[n_pix,n_ch]
        return:
            w: [n_img, n_px, n_ch]
            rho_tilde:[n_pix,n_ch]
        '''
        phi_chich = (((psi*chi).T)[:,:,None]*Phi[None,:,:]).transpose((1,0,2))
        w = self.get_w_fcn(self.get_r_fcn(rho_tilde, Phi, psi, I), lamda, estimator=self.estimator)
        denom = (np.sum((w*(phi_chich)**2).transpose((1,0,2)),1))
        arr_matrix = np.where(denom>0)       
        if(len(arr_matrix[0])>0):
            rho_tilde[arr_matrix[0],arr_matrix[1]] = (np.sum(w[:,arr_matrix[0],arr_matrix[1]]*(I[:,arr_matrix[0],arr_matrix[1]])*phi_chich[:,arr_matrix[0],arr_matrix[1]],0))/denom[arr_matrix[0],arr_matrix[1]]

        return rho_tilde

    def log_depth_update(self,I,Phi,psi,chi,rho_tilde,uv,imask,fx,fy,T_z,grad_Tz,Dx_rep,Dy_rep,z0,z_tilde,lamda):
        '''
        input:
            I: [n_imgs,n_pix,n_ch]
            Phi: [n_imgs,n_ch]
            psi: [n_imgs,n_pix]
            chi:[n_imgs,n_pix]
            rho_tilde:[n_pix,n_ch]
            uv: [2,(h*w)]
            imask: [n_px]
            T_z/grad_Tz: [n_img,n_px,3]
            Dx_rep: [n_img*n_px,n_px]
            z_tilde: [n_px,1]
        '''
        w = self.get_w_fcn(self.get_r_fcn(rho_tilde, Phi, psi, I), lamda, estimator=self.estimator)   # [n_img, n_px, n_ch]
        
        rho_rep = rho_tilde[None,:]*Phi[:,None,:]                           # [n_img, n_px, n_ch]
        D = w*np.power(chi[:,:,None]*rho_rep,2)                             # [n_img, n_px, n_ch]
       
        A = sp.spdiags((fx*T_z[:,:,0]-uv[0,imask]*T_z[:,:,2]).reshape(-1), 0, self.n_img*self.n_px, self.n_img*self.n_px)@Dx_rep +\
            sp.spdiags((fy*T_z[:,:,1]-uv[1,imask]*T_z[:,:,2]).reshape(-1), 0, self.n_img*self.n_px, self.n_img*self.n_px)@Dy_rep
                                                                            # [n_img*n_px, n_img*n_px]
        temp = (sp.spdiags((fx*grad_Tz[:,:,0]-uv[0,imask]*grad_Tz[:,:,2]).reshape(-1), 0, self.n_img*self.n_px, self.n_img*self.n_px)@Dx_rep +\
            sp.spdiags((fy*grad_Tz[:,:,1]-uv[1,imask]*grad_Tz[:,:,2]).reshape(-1), 0, self.n_img*self.n_px, self.n_img*self.n_px)@Dy_rep)@z_tilde-grad_Tz[:,:,2].reshape(-1,1)
        A = A + sp.csr_matrix((temp.reshape(-1), (np.arange(self.n_img*self.n_px), np.tile(np.arange(self.n_px), self.n_img))))
        del temp

        A = sp.vstack([A for i_ in range(self.n_ch)])                       # [n_ch*n_px*n_img, n_px]
        M = A.T@sp.spdiags(D.transpose((2,0,1)).reshape(-1), 0, self.n_img*self.n_px*self.n_ch,self.n_img*self.n_px*self.n_ch)@A
        M = M/(self.n_ch*self.n_img*self.n_px)
        dia_zeta = sp.diags((self.zeta).reshape(-1), 0)
        M = M + dia_zeta/self.n_px
        # M = M + self.zeta*sp.eye(self.n_px)/self.n_px                       # [n_px, n_px]

        rhs = chi[:,:,None]*rho_rep*(-rho_rep*psi[:,:,None] + I)*w          # [n_img, n_px, n_ch]
        rhs = A.T@rhs.transpose((2,0,1)).reshape(-1,1)/(self.n_ch*self.n_img*self.n_px)
        rhs = rhs + self.zeta*(z0 - z_tilde)/self.n_px
        del rho_rep

        # DD = sp.spdiags(M.diagonal(), 0, M.shape[0], M.shape[1])
        # LL = sp.tril(M)
        # P_L = (DD+LL)@inv(DD)@(DD+LL.transpose())
        # z_tilde = z_tilde + cg(M,rhs,M=P_L)[0].reshape(-1,1)

        z_tilde = z_tilde + spsolve(M, rhs).reshape(-1,1)

        return z_tilde

    def intensity_update(self,I,Phi,psi,rho_tilde,chi,lamda):
        '''
        712-721
        input:
            I: [n_imgs,n_pix,n_ch]
            W_idx: [n_imgs,n_pix,n_ch]
            Phi: [n_imgs,n_ch]
            psi: [n_imgs,n_pix]
            rho_tilde:[n_pix,n_ch]
            chi:[n_imgs,n_pix]
        return:
            Phi:[n_imgs,n_ch]
        '''
        r_fcn_res = self.get_r_fcn(rho_tilde,Phi,psi,I)
        w_fcn_res = self.get_w_fcn(r_fcn_res,lamda,estimator=self.estimator)
        w = np.where(w_fcn_res>=0,w_fcn_res,0)
        rho_psi_chi = psi[:,:,None]*chi[:,:,None]*rho_tilde[None,:,:]
        Phi = ((np.sum(w*I*rho_psi_chi,1)))/(np.sum(w*(rho_psi_chi)**2,1))
        return Phi

    def get_N_from_depth(self,z_tilde,M,uv,imask,fx,fy):
        '''
        input:
            z_tilde: [n_px,1]
            M: [2*n_px,px]
            uv: [2,h*w]
            imask: [n_px]
        output:
            N: [h,w,3]
        '''
        DZ = (M @ z_tilde).reshape(-1, 2)
        zx = DZ[:, 0]
        zy = DZ[:, 1]
        del DZ

        Nx = np.zeros((self.h*self.w, 1)); Nx[imask,:]=fx*zx[:,np.newaxis];  
        Ny = np.zeros((self.h*self.w, 1)); Ny[imask,:]=fy*zy[:,np.newaxis];  
        Nz = np.ones((self.h*self.w, 1));  Nz[imask,:]=(-uv[0,imask]*zx - uv[1,imask]*zy - 1)[:,np.newaxis]
        dz = np.sqrt(Nx**2+Ny**2+Nz**2)
        N = np.concatenate((Nx/dz, Ny/dz, Nz/dz), axis=1).reshape(self.h, self.w, 3)

        return N, dz

    def write_N_png(self,N, save_path):
        '''
            N: [h,w,3]
        '''
        N[:,:,2] = -N[:,:,2]
        N = ((N+1)/2*65535).astype(np.uint16)[:,:,[2,1,0]]
        N[:,:,1] = 1-N[:,:,1]
        cv2.imwrite(save_path, N)

    def identify_neighbors(self,n,m,nan_list,talks_to):
        if len(nan_list)>0:
            nan_count=nan_list.shape[0]
            talk_count=talks_to.shape[0]
            nn = np.zeros((nan_count * talk_count, 2))
            j=np.array([0,nan_count-1])
            for i in range(talk_count):
                nn[j[0]:j[1]+1,:]=nan_list[:,1:3]+np.tile(talks_to[i,:],(nan_count,1))
                j+=nan_count

            L=np.zeros((nn.shape[0],1))
            L[np.where(nn[:,0]<0)]=1
            L[np.where(nn[:, 0] > n-1)] = 1
            L[np.where(nn[:, 1] < 0)] = 1
            L[np.where(nn[:, 1] > m-1)] = 1

            nn=nn[np.where(L!=1)[0],:]

            neighbors_list=np.concatenate(((nn[:,1]*n+nn[:,0]).reshape(-1,1),nn),axis=1)

            neighbors_list=np.unique(neighbors_list,axis=0)

            list_neighbors=neighbors_list.tolist()
            list_nan=nan_list.tolist()
            temp=[]
            for i in range(neighbors_list.shape[0]):
                if list_neighbors[i] not in list_nan:
                    temp.append(list_neighbors[i])

            neighbors_list=np.array(temp)
        else:
            neighbors_list=np.array([])
        return neighbors_list

    def inpaint_nans(self, A, method=0):
        n,m=A.shape
        A_new=A.T.reshape(-1,1)
        nm=n*m
        k=np.isnan(A_new)
        nan_list=np.where(k==True)[0].reshape(-1,1)

        nr,nc=np.array(np.where(k==True))                 #[nr.nc]对应nan值坐标
        nr=nr.reshape(-1,1)
        nc=nc.reshape(-1,1)

        known_list=np.where(k==False)[0]
        nan_count=len(nan_list)

        nan_list=np.concatenate((nan_list,nr,nc),axis=1)

        if method not in [0,1,2,3,4,5]:
            raise '''method must be one of [0,1,2,3,4,5]'''

        if method==0:
            if m==1 or n==1:
                work_list=nan_list[:,0].reshape((-1,1))
                work_list=np.unique(np.concatenate((work_list,work_list-1,work_list+1),axis=0)).reshape(-1,1)
                work_list=work_list[np.where(work_list>0)]
                work_list=work_list[np.where(work_list<nm-1)]
                nw=len(work_list)

                u=np.array([i for i in range(nw)]).reshape(1,-1).T

                V=np.tile(np.array([[1,-2,1]]),(nw,1))
                I=np.tile(u,(1,3))
                J=np.tile(work_list,(1,3))+np.tile(np.array([-1,0,1],(len(work_list),1)))
                fda=sp.csr_matrix((V, (I, J)), shape=(nw,nm),dtype= np.int32)
            else:
                talks_to= np.array([[-1,0],[0,-1],[1,0],[0,1]])
                neighbors_list = self.identify_neighbors(n, m, nan_list, talks_to)
                all_list=np.concatenate((nan_list,neighbors_list),axis=0)

                L=np.where(all_list[:, 1] < n-1) or np.where(all_list[:, 1] > 0) 
                nl=len(L[0])
                if nl>0:
                    V = np.tile(np.array([[1, -2, 1]]), (nl, 1))
                    I = np.tile(all_list[L,0], (1, 3))
                    J = np.tile(all_list[L,0], (1, 3)) + np.tile(np.array([-1, 0, 1], (nl, 1)))
                    fda=sp.csr_matrix((V, (I, J)), shape=(nm,nm),dtype= np.int32)
                else:
                    fda=0

                L=np.where(0<all_list[:,2].all()<m-1)
                nl=len(L[0])
                if nl>0:
                    V = np.tile(np.array([[1, -2, 1]]), (nl, 1))
                    I = np.tile(all_list[L, 0], (1, 3))
                    J = np.tile(all_list[L, 0], (1, 3)) + np.tile(np.array([-n, 0, n], (nl, 1)))
                    fda = fda+sp.csr_matrix((V, (I, J)), shape=(nm, nm), dtype=np.int32)

                # eliminate knowns
                rhs=-fda[:,known_list-1]*A.T.reshape(-1,1)[known_list]
                fda_temp=fda[:,nan_list[:,0]]
                k=np.where(fda_temp.data!=0)

                B=A.T.reshape(-1,1)
                B[nan_list[:,0]]=rhs[k]/fda[k,nan_list[:,0]]
                B=B.reshape(m,n).T
        return B

    def shadow_Cal(self,images,mask0):

            
            sh_vars = np.sum(np.var(images,axis=0),axis=2)*mask0
  
            sh_vars = (sh_vars-np.min(sh_vars))/(np.max(sh_vars)-np.min(sh_vars))

            # sh_vars = sh_vars*255
            
            lam_vec = np.power(self.zeta,sh_vars)+50
            # print(max(lam_vec))
            

            # cv2.imwrite(imgs_save_path+'sh_vars.png', sh_vars*255)

            return lam_vec

    def main_loop(self, images, K, light_p, light_d, z0, zeta_vec=None,mask0=None, mu=1, imgs_save_path=None):
        '''
        input:
            images: [n_img, H, W, n_ch]
            K: [3,3]
            light_p: [n_img, 3] 光源位置
            light_d: [n_img, 3] 光源方向
            z0: 场景平均深度
            mask: [H,W] 图像mask, 可选
            mu: 光源各向异性参数
        '''
        #####
        self.n_img, h, w, self.n_ch = images.shape
        max_I = np.max(images); images = images.astype(np.float32)/max_I
        Phi = 1/max_I*np.ones((self.n_img, self.n_ch))

        fx_ = K[0,0]; fy_ = K[1,1]; x0_ = K[0,2]; y0_ = K[1,2]
        if mask0 is None:
            mask0 = np.ones((h, w))
        if zeta_vec is None:
            zeta_vec = np.ones((h, w))*self.zeta

        #######
        lamda_ref = self.lamda
        fx_ref = fx_; fy_ref = fy_; x0_ref = x0_; y0_ref = y0_

        mask_ref = mask0.astype(np.float32)
        I_ref = images; _, h_ref, w_ref, _ = I_ref.shape
        del images
        z0_ref = z0.copy(); z_final = z0.copy()
        z_final = cv2.resize(z_final, [w_ref,h_ref], interpolation=cv2.INTER_LINEAR)
        rho_final = np.zeros((h, w, self.n_ch))   # 0初始化是否合适
        #######
        scales_list = [np.power(2,self.scales-x-1) for x in range(self.scales)]
        scale_0 = int(h/z0.shape[0])
        

        for ratio in scales_list:
            fx = fx_ref/ratio; fy = fy_ref/ratio; x0 = x0_ref/ratio; y0 = y0_ref/ratio
            mask = mask_ref[::ratio, ::ratio].copy()
            
            if ratio>=scale_0:
                z0 = z0_ref[::ratio//scale_0,::ratio//scale_0]
            else:
                z0 = cv2.resize(z0_ref, (0,0),fx=scale_0//ratio,fy=scale_0//ratio)

            self.h, self.w = mask.shape
            imask = np.where(mask.reshape(-1)>0)[0];  self.n_px = imask.shape[0]

            
            self.zeta = (zeta_vec[::ratio, ::ratio].copy()).reshape(-1,1)[imask,:]
            print(np.max(self.zeta),np.min(self.zeta))
                # sys.exit('here')
            

            uv = np.array(np.where(mask != None), dtype=np.float32)[[1,0],:]
            uv[0,:] = uv[0,:] - x0; uv[1,:] = uv[1,:] - y0
            I = I_ref[:, ::ratio, ::ratio, :].copy()
            I = I.reshape(self.n_img, -1, self.n_ch)[:, imask, :]
            
            if self.estimator !='Lp':
                lamda = lamda_ref*np.median(np.abs(I-np.median(I)))
            else:
                lamda = lamda_ref

            z0 = np.log(z0).reshape(-1,1)[imask,:]
            z = z_final[::ratio,::ratio].copy()
            z_tilde = np.log(z.reshape(-1,1)[imask,:])

            M, Dx, Dy = self.make_gradient(mask)
            # M, Dx, Dy = self.create_Dx_Dy([self.h, self.w])
            Dx_rep = sp.vstack([Dx for i_ in range(self.n_img)])
            Dy_rep = sp.vstack([Dy for i_ in range(self.n_img)])
            N, dz = self.get_N_from_depth(z_tilde,M,uv,imask,fx,fy)
            # if ratio==scales_list[0]:
            #     self.write_N_png(N, imgs_save_path+'N_oz.png')
            # sys.exit('here')
            
            rho = rho_final[::ratio, ::ratio].copy()
            rho_tilde = rho.reshape(-1, self.n_ch)
            rho_tilde= rho_tilde/dz.reshape(-1)[:,None]
            rho_tilde = rho_tilde[imask,:]

            T_z, grad_Tz = self.get_t_fcn(z_tilde, light_p, light_d, mu, (uv[0,imask]/fx)[:, np.newaxis], (uv[1,imask]/fy)[:, np.newaxis])
            psi = self.get_shading_fcn(T_z, z_tilde, M, self.get_Q(fx, fy, uv[0,imask], uv[1,imask]))
            energy = self.get_J_fcn(rho_tilde, Phi, psi, I, lamda); energy = energy/(self.n_px*self.n_img*self.n_ch)
            energy = energy + self.zeta*np.sum((z_tilde-z0)**2)/self.n_px

            tab_nrj = [energy]
            for it in range(self.maxit):
                chi = self.get_chi_fcn(psi, self.self_shadows)                    # [n_img, n_px]
                #rho_update
                rho_tilde = self.pseudo_albedo_update(I,Phi,psi,chi,rho_tilde.copy(),lamda)
                #log_z update
                z_tilde = self.log_depth_update(I,Phi,psi,chi,rho_tilde,uv,imask,fx,fy,T_z,grad_Tz,Dx_rep,Dy_rep,z0,z_tilde.copy(),lamda)
                z.reshape(-1,1)[imask] = np.exp(z_tilde.copy())
                N_old = N.copy()
                N, dz = self.get_N_from_depth(z_tilde.copy(),M,uv,imask,fx,fy)

                ############# intensity update
                Phi = self.intensity_update(I,Phi,psi,rho_tilde.copy(),chi,lamda)
                rhoch = rho
                rhoch.reshape(-1,rho.shape[2])[imask,:] = rho_tilde*dz[imask,:]
                max_val = np.median(rhoch.reshape(-1,rho.shape[2])[imask,:],axis=0)+8*np.std(rhoch.reshape(-1,rho.shape[2])[imask,:],axis=0)
                for i in range(rho.shape[2]):
                    rhoch[:,:,i] = np.where(rhoch[:,:,i]>max_val[i],max_val[i],rhoch[:,:,i])        
                rhoch = np.where(rhoch<0,0,rhoch)
                rho = rhoch

                # Covergence test
                T_z, grad_Tz = self.get_t_fcn(z_tilde.copy(), light_p, light_d, mu, (uv[0,imask]/fx)[:, np.newaxis], (uv[1,imask]/fy)[:, np.newaxis])
                psi = self.get_shading_fcn(T_z, z_tilde.copy(), M, self.get_Q(fx, fy, uv[0,imask], uv[1,imask]))
                energy_new = self.get_J_fcn(rho_tilde.copy(), Phi, psi, I, lamda); energy_new = energy_new/(self.n_px*self.n_img*self.n_ch)
                relative_diff = np.abs(energy_new-energy)/energy_new
                normal_diff = np.rad2deg(np.real(np.arccos(np.sum(N_old*N,2))))
                normal_residual = np.median(normal_diff.reshape(-1,1)[imask])
                diverged = energy_new>energy
                energy = energy_new
                tab_nrj.append(energy)

                if it>1 :
                    if relative_diff<self.tol:
                        print('CONVERGENCE REACHED: ENERGY EVOLUTION IS LOW ENOUGH')
                        break
                    if normal_residual<self.tol_normals:
                        print('CONVERGENCE REACHED: ENERGY EVOLUTION IS LOW ENOUGH')
                        break
                    if diverged:
                        print('STOPPED: DIVERGENT BEHAVIOR DETECTED')
                        break
                elif it==self.maxit:
                    print('STOPPED: MAX NUMBER OF ITERATIONS REACHED')
                    break
            
            if imgs_save_path and self.save_current_result:
                self.write_N_png(N.copy(), imgs_save_path+'N_{}.png'.format(ratio))
                cv2.imwrite(imgs_save_path+'albedo_{}.png'.format(ratio), (rho[:,:,[0,1,2]]/np.max(rho)*65535).astype(np.uint16))
                np.save(imgs_save_path+'depth_{}.npy'.format(ratio), z)

            # Update final result by interpolation
            if ratio>1:
                print('upscaling for the next level in the pyramid...')
                z_final = cv2.resize(z.copy(),[w_ref,h_ref], interpolation=cv2.INTER_LINEAR)
                rho_final = cv2.resize(rho.copy(),[w_ref,h_ref], interpolation=cv2.INTER_LINEAR)
        Phi = Phi*max_I
        print('Max Height:', np.max(z_final)-np.min(z_final))
        return (rho[:,:,[0,1,2]]/np.max(rho)*65535).astype(np.uint16), N ,z

    def run(self,dep=None):
       
        obj_name = self.obj_name
        size = self.size
        z0 = self.z0

        mask_path = f'testData/0909_50_zhuantou/{obj_name}/mask.png'
        K_path = f'testData/0909_50_zhuantou/{obj_name}/camera.txt'  # 相机内参
        lights_path = f'testData/0909_50_zhuantou/{obj_name}/light.txt'  # 相机坐标系下的光源坐标
        imgs_path = f'testData/0909_50_zhuantou/{obj_name}/imgs/'  # 图片文件夹
        imgs_save_path = f'testData/0909_50_zhuantou/{obj_name}/out/'  # 结果保存路径
    
        # np.save(imgs_save_path+'params.npy', self.params)    
        info_json = json.dumps(self.params,sort_keys=False, indent=4, separators=(',', ': '))
        f = open(imgs_save_path+'params.json', 'w')
        f.write(info_json)
  

        K = np.loadtxt(K_path)
        light_p = np.loadtxt(lights_path)
        light_d = light_p - np.array([[0,0,z0]]); light_d = light_d / np.linalg.norm(light_d, axis=1)[:,None]
        imgs_dir = os.listdir(imgs_path) ; imgs_dir.sort(key = lambda x: int(x[13:-7]))     # 注意图片名称
        
        imgs_dir_list = [imgs_path+x for x in imgs_dir]

        imgs = self.load_imgs(imgs_dir_list, size=self.size, K=K, cos4a=self.cos4a)

        
        K[:2,:] /= size
        mask0 = (cv2.imread(mask_path,cv2.IMREAD_GRAYSCALE)[::size,::size]/255).astype(int)

        if self.use_depth:
            z0_ = dep[::size,::size]
        else:
            z0_ = np.ones((imgs.shape[1]//2,imgs.shape[2]//2), np.float32)*z0


        # index = [4,5,6,7,8,10,20,21,22,23,24,25,26,36,37,38,39,40,41,42,52,53,54,55,56,57,58]
        # light_p = light_p[1:84:2,:]
        # light_d = light_d[1:84:2,:]
        # imgs = imgs[1:84:2,:,:,:]

        if self.cal_shadow:
            zeta_vec = self.shadow_Cal(imgs,mask0)
        else:
            zeta_vec = None

        t1 = time.time()
        rho, N , z= self.main_loop(imgs, K, light_p, light_d, z0_,zeta_vec, mask0 = None,mu=0, imgs_save_path=imgs_save_path)
        t2 = time.time()
        cv2.imwrite(imgs_save_path+'albedo.png', rho)
        self.write_N_png(N, imgs_save_path+'N.png')
        print((t2-t1)/60, imgs.shape)
        return z,rho
        


if __name__ == "__main__":
    # 所有的输入文件默认在testData下
    # 文件排列如下
    # 文件名
    # |----imgs
    # |----out
    # |----camera 相机内参
    # |----depth.npy
    # |----light.mat 相机坐标系下的光源位置
    # |----mask.png
    dep=np.load('testData/0909_50_zhuantou/side1_HR/depth.npy')
    Semi_calib('data/config_120_mid.json').run(dep)
    
