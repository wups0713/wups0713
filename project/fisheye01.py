# collection of functions used for fisheye images and virtual PTZ

import cv2
import numpy as np
import math


# modified from CHX003x.m (equal-distance model)
# convert physical coordinates to fisheye image coordinates
# origin at camera center; z<0 always (+z along camera axis); th=0 along +x axis
# output pZ is depth (distance from camera)
# inputs pc and pr are center and radius of outer circle
# input a00 (in rad) is the view angle at the outer circle relative to the -z axis (usually about 90 deg)
def FE_world2img(x, y, z, pc, pr, a00=np.pi/2):
    d = np.sqrt(np.multiply(x, x) + np.multiply(y, y))
    th = np.arctan2(y, x)
    az = np.arctan2(d, z)
    r = pr / a00 * az
    pX = np.multiply(r, np.cos(th)) + pc[0]
    pY = np.multiply(r, np.sin(th)) + pc[1]
    pZ = np.sqrt(np.multiply(d, d) + np.multiply(z, z))
    return pX, pY, pZ


# virtual PTZ (precompute the transform)
# all angles in radians
# FE image parameters:
#   sz0: fisheye image size
#   pfc: (x,y) center of outer circle, as a ratio of sz0
#   pfr: radius of outer circle, as a ratio of sz0
#   a00: half-view-angle (close to pi/2) at the outer circle
# rectified image parameters
#   ua: angle between image-center ray and camera optical axis
#   za: rotation angle (counter-clockwise relative to +y) of image-center ray around cmaera optical axis
#   hxa, hya: full-width horizontal and vertical view-angles
#   hxs, hys: full-width image size in pixels
# output QK contains mapping from the virtual view image to the original FE image
class FEPTZ:
    def __init__(self, sz0, pfc=np.array([.5,.5]), pfr=.5, a00=np.pi/2,
                 ua=0, za=0, hxa=np.pi/2, hya=np.pi/2, hxs=256, hys=256):
        self.sz0, self.pfc, self.pfr, self.a00, self.ua, self.za, self.hxa, self.hya, self.hxs, self.hys \
                = sz0, pfc, pfr, a00, ua, za, hxa, hya, hxs, hys
        self.makePTZ()
    
    def makePTZ(self):
        pc = self.pfc * self.sz0
        pr = self.pfr * self.sz0
        # axis of rectified image in camera frame (-z is camera axis)
        ua_sin = math.sin(self.ua)
        ua_cos = math.cos(self.ua)
        za_sin = math.sin(self.za)
        za_cos = math.cos(self.za)
        if self.ua == 0:
            ux = np.array([1, 0, 0])
            uy = np.array([0, 1, 0])
            uz = np.array([0, 0, 1])
        else:
            uz = np.array([-ua_sin*za_sin, -ua_sin*za_cos, ua_cos])
            ux = np.cross(np.array([0, 0, 1]), uz)
            ux = ux / np.linalg.norm(ux)
            uy = np.cross(uz, ux);
        # make grid of 3-d points in camera frame for the image pixels
        t = math.tan((self.hxa/2))
        tx = np.linspace(-t, t, self.hxs)
        t = math.tan((self.hya/2))
        ty = np.linspace(-t, t, self.hys)
        txx, tyy = np.meshgrid(tx, ty)
        tzz = np.ones([int(self.hys), int(self.hxs)])
        ttx = txx * ux[0] + tyy * uy[0] + tzz * uz[0]
        tty = txx * ux[1] + tyy * uy[1] + tzz * uz[1]
        ttz = txx * ux[2] + tyy * uy[2] + tzz * uz[2]
        # get image coordinates
        px, py, _ = FE_world2img(ttx, tty, ttz, pc, pr, self.a00)
        self.pX, self.pY = np.round(px), np.round(py)
        q = np.flatnonzero(np.logical_and(np.logical_and(self.pX >= 0, self.pX < self.sz0), 
                                          np.logical_and(self.pY >= 0, self.pY < self.sz0)))
        k1 = np.ravel(self.pX)[q] + np.ravel(self.pY)[q] * self.sz0
        self.k1c = np.ravel(np.array([k1*3, k1*3+1, k1*3+2])).astype(int) # linear indices in source (color)
        self.k2c = np.ravel(np.array([q*3, q*3+1, q*3+2])).astype(int) # linear indices in destination (color)
        #print(np.shape(k1c), np.shape(k2c), np.shape(q), np.shape(k1))


