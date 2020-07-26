import FEX002CX as fx 
import cv2

def one_to_two(img_size_i,img_size_o,img):
    QK, QKr, QKL1, QKL2, hxs, hys = fx.get_view_transform(img_size_o,img_size_i)
    imr_out = fx.make_composite_view(img, 416, 1056, QK, QKL1, QKL2, hxs, hys)
    return imr_out

def combine(img_size_o,img_size_i,list):
    QK, QKr, QKL1, QKL2, hxs, hys = fx.get_view_transform(img_size_o,img_size_i)
    ngroup = len(QKr)
    ndet = [len(tt) for tt in list]
    det_out = []
    for i in range(ngroup):
        x = []
        for j in range(ndet[i]):
            top     = list[i][j][2][1] - list[i][j][2][3] 
            left    = list[i][j][2][0] - list[i][j][2][2] 
            bottom  = list[i][j][2][1] + list[i][j][2][3] 
            right   = list[i][j][2][0] + list[i][j][2][2] 
            x.append([[top ,left ,bottom ,right], list[i][j][1]])
        det_out.append(x)
    det_out_FE, kdet = fx.combine_detections(img_size_o,det_out,QKr,hxs,hys)
    return det_out_FE, kdet