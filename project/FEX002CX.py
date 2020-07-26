"""
This is modified from FEX002B, including only the rectification parameter generation
part and the detection integration part. The detections are expected to be made separately
using, for example, YOLO.
Some parameter settings are specific for the ITRI_storage1 dataset.
"""

import sys
import cv2
import numpy as np
import math
from shapely.geometry import Polygon
from fisheye01 import FEPTZ


# helper function to group several same-size images into a single large image
# input IM: first dimension is for all the images (batch dimension)
def group_img(IM, ncols):
    N, sy, sx = np.shape(IM)[0:3]
    nrows = int(np.ceil(N/max(1,ncols)))
    s = list(np.shape(IM)[1:])
    ss = [s[0]*nrows, s[1]*ncols]
    ss = ss + s[2:]
    #print(ss, N, sy, sx)
    im = np.zeros(ss, IM.dtype)
    for i in range(N):
        x0 = (i%ncols)*sx
        y0 = (i//ncols)*sy
        im[y0:(y0+sy), x0:(x0+sx), ...] = IM[i, ...]
    return im


# basic fisheye image parameters (here only the default)
def get_img_set_param():
    pfc = np.array([.5, .5]) # relative position of circle center in the image
    pfr = .5 # relative radius of circle center in the image
    # pfc and pfr are relative to input image height
    a00 = math.radians(90) # view angle of circle
    return pfc, pfr, a00


# parameters for generating composite rectified views
# input 'imsz' is the size of the generated composite image used for YOLO detection
def get_view_param(imsz):
    ua = np.radians([36,36])
    hxa = np.radians([48,48])
    hya = np.radians([96,96])
    hxs = [int(imsz/4), int(imsz/4)]
    hys = [int(imsz/2), int(imsz/2)]
    pA = [np.arange(0,15,2)*(np.pi/8), np.arange(1,16,2)*(np.pi/8)]
    return ua, hxa, hya, hxs, hys, pA


# compute transforms for rectified views,
# to be used for all input fisheye images of the same size
# input 'sy0' is the height of expected input fisheye images
def get_view_transform(imsz, sy0):
    ua, hxa, hya, hxs, hys, pA = get_view_param(imsz)
    ngroup = len(pA) # number of composite images per source image
    pfc, pfr, a00 = get_img_set_param()
    QK, QKr, QKL1, QKL2 = [], [], [], []
    for j1 in range(ngroup): # loop over composite images
        QK.append([])
        QKr.append([])
        tk1, tk2 = [], []
        nstep = pA[j1].size
        for j2 in range(nstep): # loop over rectified views
            QK[j1].append( FEPTZ(sy0, pfc, pfr, a00, ua[j1], pA[j1][j2], \
                                 hxa[j1], hya[j1], hxs[j1], hys[j1]) )
            QKr[j1].append( FEPTZ(imsz, pfc, pfr, a00, ua[j1], pA[j1][j2], \
                                  hxa[j1], hya[j1], hxs[j1], hys[j1]) )
            tk1 = tk1 + QK[j1][j2].k1c.tolist()
            tk2 = tk2 + (QK[j1][j2].k2c + hys[j1]*hxs[j1]*3*j2).tolist()
        QKL1.append(np.array(tk1, np.int32))
        QKL2.append(np.array(tk2, np.int32))
    return QK, QKr, QKL1, QKL2, hxs, hys


# create images of composite views an input image (should be square with size sy0*sy0)
# output is a list of images from the same input image
def make_composite_view(im0, imsz, sy0, QK, QKL1, QKL2, hxs, hys):
    ngroup = len(QK)
    imr_out = []
    for j1 in range(ngroup):
        nstep = len(QK[j1])
        imr = np.zeros((nstep, hys[j1], hxs[j1], 3), dtype=np.ubyte)
        np.ravel(imr)[QKL2[j1]] = np.ravel(im0)[QKL1[j1]]
        imr[:,[0,1,hys[j1]-2,hys[j1]-1],:,:] = 0
        imr[:,:,[0,1,hxs[j1]-2,hxs[j1]-1],:] = 0
        imr_out.append( group_img(imr, int(np.ceil(nstep/2))) )
    return imr_out


# create images of composite views for a list of image files in the same folder
# input 'fnlist' is a list of strings of image files (not including .jpg extension)
def make_composite_view_folder(spath_in, spath_out, fnlist, imsz):
    N = len(fnlist) # number of files
    sx0_old, sy0_old = 0, 0
    for i in range(N):
        im0 = cv2.imread(spath_in + '\\' + fnlist[i] + '.jpg')
        sy0, sx0 = np.shape(im0)[0:2]
        if (sx0 != sx0_old) | (sy0 != sy0_old): # set up rectified view parameters
            sx0_old, sy0_old = sx0, sy0
            gx = (sx0 - sy0) // 2
            QK, QKr, QKL1, QKL2, hxs, hys = get_view_transform(imsz, sy0)
            ngroup = len(QK)
        im0t = im0[:, gx:(gx+sy0), :]  # make the input image square
        imr_out = make_composite_view(im0t, imsz, sy0, QK, QKL1, QKL2, hxs, hys)
        # create composite images
        for j1 in range(ngroup):
            cv2.imwrite(spath_out + '\\' + fnlist[i] + '___g{0:02d}.jpg'.format(j1), imr_out[j1])


# non-maximum suppression
# output is a list of indices of detections retained after NMS
def do_NMS(detBB, thr_iou, thr_contain):
    n = len(detBB)
    BB, BBsc, BBarea = [], [], []
    for i in range(n):
        fx, fy, sc = detBB[i][:3]
        poly = Polygon([(fx[0],fy[0]), (fx[1],fy[1]), (fx[2],fy[2]), (fx[3],fy[3])])
        BB.append(poly)
        BBarea.append(poly.area)
        BBsc.append(sc)
    BM_iou = np.zeros((n,n))
    BM_contain = np.zeros((n,n))
    for i1 in range(n-1):
        for i2 in range(i1+1,n):
            a = BB[i1].intersection(BB[i2]).area
            iou = a / (BBarea[i1] + BBarea[i2] - a)
            BM_iou[i1,i2], BM_iou[i2,i1] = iou, iou
            BM_contain[i1,i2] = a / BBarea[i1]
            BM_contain[i2,i1] = a / BBarea[i2]
    kdet_keep = []
    sc0 = np.reshape(np.array(BBsc), (1,-1))
    for i in range(n):
        if np.count_nonzero(np.logical_and(sc0 > BBsc[i],
                            np.logical_or(BM_iou[i,:] >= thr_iou,
                                          BM_contain[i,:] >= thr_contain))) == 0:
            kdet_keep.append(i)
    return kdet_keep


# integrate detection in the rectified views into the fisheye image frame
# input 'det_out' (a list) contains all the detections from the composite-rectified-view images:
#   det_out[j1]: the list of detections from image no. j1
#   det_out[j1][j2]: the j2th detection in det_out[j1], and contains a tuple
#     det_out[j1][j2][0]: the bounding box (top, bottom, left, right)
# input 'QKr' contains information for mapping from the rectified views to the fisheye frame
#   and is computed in function get_view_transform
# output 'det_FE_out' (a list) has one-on-one correspondence with det_out
#   det_FE_out[0] and det_FE_out[1]: lists of x and y coordinates (in fisheye image) of the corners
#   det_FE_out[2]: scores
# output 'kdet' is the list containing indices of detections retained after NMS
def combine_detections(imsz, det_out, QKr, hxs, hys):
    det_out_FE = []
    ngroup = len(QKr)
    ndet = [len(tt) for tt in det_out]
    for j1 in range(ngroup):
        for j2 in range(ndet[j1]):
            sc = det_out[j1][j2][1]
            b = det_out[j1][j2][0]
            xc, yc = int((b[1] + b[3]) / 2), int((b[0] + b[2]) / 2) # BB center in image
            ipatch = (xc // hxs[j1]) + (yc // hys[j1]) * (imsz // hxs[j1]) # patch index
            dx, dy = (xc // hxs[j1]) * hxs[j1], (yc // hys[j1]) * hys[j1] # image-to-patch coordinate offsets
            x1t, y1t, x2t, y2t = int(b[1]), int(b[0]), int(b[3]), int(b[2])
            # limit the BB to within patch
            px1, px2 = max(0, x1t-dx), min(hxs[j1]-1, x2t-dx)
            py1, py2 = max(0, y1t-dy), min(hys[j1]-1, y2t-dy)
            # reduce scores for BBs too close to or cropped by the patches
            ar = float((px2 - px1) * (py2 - py1)) / ((x2t - x1t) * (y2t - y1t)) # area ratio
            g = imsz * .01
            ee0 = 2 * (px2 - px1 + py2 - py1)
            gfx1, gfx2 = max(0, min(1, px1 / g)), max(0, min(1, (hxs[j1]-1-px2) / g))
            gfy1, gfy2 = max(0, min(1, py1 / g)), max(0, min(1, (hys[j1]-1-py2) / g))
            ee = (gfx1 + gfx2) * (px2 - px1) + (gfy1 + gfy2) * (py2 - py1)
            scp = sc * ar * (ee / ee0)
            # compute BB coordinates in FE
            pxc, pyc = int((px1 + px2) / 2), int((py1 + py2) / 2) # BB center in patch
            ptx, pty = [pxc, pxc, px1, px2], [py1, py2, pyc, pyc] # BB side center points in patch
            #print(j1, j2, ipatch, ptx, pty, np.shape(QKr[j1][ipatch].pX))
            fxc, fyc = QKr[j1][ipatch].pX[pyc,pxc], QKr[j1][ipatch].pY[pyc,pxc] # BB center in FE
            ftx, fty = QKr[j1][ipatch].pX[pty,ptx], QKr[j1][ipatch].pY[pty,ptx] # BB side center points in FE
            fh = np.linalg.norm((np.array([ftx[0],fty[0]]) - np.array([ftx[1],fty[1]]))) # BB height in FE
            fw = np.linalg.norm((np.array([ftx[2],fty[2]]) - np.array([ftx[3],fty[3]]))) # BB width in FE
            fc0 = imsz / 2
            u1 = np.array([fxc, fyc]) - fc0
            if np.linalg.norm(u1) == 0:
                u1 = np.array([ftx[1], fty[1]]) - fc0
            u1 = u1 / np.linalg.norm(u1) ## unit vector of BB (radial direction)
            u2 = [u1[1], -u1[0]] ## unit vector of BB (tangential direction)
            a1 = np.array([-1,-1,1,1]) * (fw/2)
            a2 = np.array([-1,1,1,-1]) * (fh/2)
            fx, fy = np.array(a1*u2[0] + a2*u1[0] + fxc, np.int32), \
                     np.array(a1*u2[1] + a2*u1[1] + fyc, np.int32) # BB corners in FE
            det_out_FE.append((fx, fy, scp, fxc, fyc, u1, u2, fw, fh))
    # NMS
    kdet = do_NMS(det_out_FE, .5, .6)
    return det_out_FE, kdet
