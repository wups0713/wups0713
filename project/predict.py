import math
import shapely
from shapely.geometry import Polygon,MultiPoint
import numpy as np
import pickle

def to_xy (t, width, height):
    x1 = int (round (float (t[0]) * width, 0) - round (float (t[2]) * width, 0) / 2.0)
    y1 = int (round (float (t[1]) * height, 0) - round (float (t[3]) * height, 0) / 2.0)
    x2 = int (round (float (t[0]) * width, 0) + round (float (t[2]) * width, 0) / 2.0)
    y2 = int (round (float (t[1]) * height, 0) + round (float (t[3]) * height, 0) / 2.0)
    return x1, y1, x2, y2

def two_to_four (a, b, size):
    a[0] -= size / 2
    a[1] = -a[1] + size / 2
    b[0] -= size / 2
    b[1] = -b[1] + size / 2
    center = [(a[0] + b[0]) / 2, (a[1] + b[1]) / 2]

    cos1 = (a[0] * center[0] + a[1] * center[1]) / (math.sqrt (a[0]**2 + a[1]**2) * math.sqrt (center[0]**2 + center[1]**2))
    cos2 = (b[0] * center[0] + b[1] * center[1]) / (math.sqrt (b[0]**2 + b[1]**2) * math.sqrt (center[0]**2 + center[1]**2))
    sin1 = math.sqrt (1 - cos1**2)
    sin2 = math.sqrt (1 - cos2**2)
    cos2_1 = cos1**2 - sin1**2
    cos2_2 = cos2**2 - sin2**2
    sin2_1 = 2 * sin1 * cos1
    sin2_2 = 2 * sin2 * cos2
    cross1 = a[0] * center[1] - a[1] * center[0]
    cross2 = b[0] * center[1] - b[1] * center[0]

    a1 = 0
    b1 = 0
    if cross1 > 0:
        a1 = [round (cos2_1 * a[0] - sin2_1 * a[1], 6), round (sin2_1 * a[0] + cos2_1 * a[1], 6)]
    else:
        a1 = [round (cos2_1 * a[0] + sin2_1 * a[1], 6), round (-sin2_1 * a[0] + cos2_1 * a[1], 6)]

    if cross2 > 0:
        b1 = [round (cos2_2 * b[0] - sin2_2 * b[1], 6), round (sin2_2 * b[0] + cos2_2 * b[1], 6)]
    else:
        b1 = [round (cos2_2 * b[0] + sin2_2 * b[1], 6), round (-sin2_2 * b[0] + cos2_2 * b[1], 6)]
    
    a[0] += size / 2
    a[1] = -a[1] + size / 2
    b[0] += size / 2
    b[1] = -b[1] + size / 2
    a1[0] += size / 2
    a1[1] = -a1[1] + size / 2
    b1[0] += size / 2
    b1[1] = -b1[1] + size / 2
    return [a[0], a1[0], b[0], b1[0]], [a[1], a1[1], b[1], b1[1]]

def person_IOU (a, b):
    poly1 = Polygon (np.array (a).transpose ().tolist ()).convex_hull
    poly2 = Polygon (np.array (b).transpose ().tolist ()).convex_hull

    if poly1.intersects (poly2):
        inter_area = poly1.intersection (poly2).area
        return float (inter_area) / (poly1.area + poly2.area - inter_area)
    else:
        return 0

def connected (a, b):
    if a[2] < b[0] or a[0] > b[2] or a[3] < b[1] or a[1] > b[3]:
        return False
    return True

def Predict ():
    import os
    import re
    import cv2
    import csv
    import time
    import configparser
    import split_combine

    import darknet as dn
    import map

    # read setting
    parser = configparser.ConfigParser (inline_comment_prefixes="#")
    parser.read ("setting.txt")

    # argument
    load_label = int (parser.get ("arguments", "load_label"))
    print_info = int (parser.get ("arguments", "print_info"))
    do_person = int (parser.get ("arguments", "do_person"))
    draw_label = int (parser.get ("image", "draw_label"))
    draw_predict = int (parser.get ("image", "draw_predict"))
    output_txt = int (parser.get ("arguments", "output_txt"))
    store_result = int (parser.get ("arguments", "store_result"))
    show_result = int (parser.get ("arguments", "show_result"))
    write_log = int (parser.get ("arguments", "write_log"))
    data_type = int (parser.get ("arguments", "data_type"))
    
    # path
    cfg_path = ""
    weight_path = ""
    data_path = ""
    list_path = ""
    log_path = "log.txt"
    pkl = ""

    for i in os.listdir ("setting"):
        if re.match (".*\.cfg$", i):
            cfg_path = str.encode ("setting/" + i)
        if re.match (".*\.weights$", i):
            weight_path = str.encode ("setting/" + i)
        if re.match (".*\.data$", i):
            data_path = str.encode ("setting/" + i)
        if re.match (".*\.txt$", i):
            list_path = "setting/" + i
        if re.match (".*\.pkl$", i):
            pkl = "setting/" + i

    # for image
    img_size = int (parser.get ("image", "img_size"))

    # for video
    video_path = parser.get ("video", "video_path")

    # for result
    result_dir = parser.get ("result", "result_dir")

    # load config
    net = dn.load_net (cfg_path, weight_path, 0)
    meta = dn.load_meta (data_path)
    net1 = None
    meta1 = None
    if do_person:
        net1 = dn.load_net (b"cfg/yolov3.cfg", b"yolov3.weights", 0)
        meta1 = dn.load_meta (b"cfg/coco.data")

    # load pkl
    person_label = None
    if do_person:
        person_label = pickle.load (open (pkl, "rb"))

    # variable
    img_count = 0                                                                                       ### total number of images
    detect_count = 0                                                                                    ### total number of predictions
    label_count = 0                                                                                     ### total number of labels
    label_count_ = 0                                                                                    ### total number of labels (person)
    total_time = 0                                                                                      ### total time of yolo

    IOU = []
    percent = []
    IOU_person = []
    percent_person = []
    dic = {}

    total = 0
    start = time.time ()

    # for image
    if data_type == 0:
        # load list
        f = open (list_path, "r")
        for i in f:
            total += 1
        f.close ()

        iou_file = open ("iou.txt", "a+")
        count_file = open ("count.txt", "a+")

        f = open (list_path, "r")
        for i in f:
            i = i.rstrip("\r\n ")

            # get image path
            file_name = i.split (".") [0].split ("\\") [-1]
            image_path = i
            label_path = i.split (".") [0] + ".txt"

            # load image
            img = cv2.imread (image_path)
            img_height = float (img.shape[0])
            img_width = float (img.shape[1])

            # transfer image
            img1 = None
            img2 = None
            if do_person:
                img_list = split_combine.one_to_two (img_height, 416, img)
                rgb_frame1 = cv2.cvtColor(img_list [0], cv2.COLOR_BGR2RGB)
                img1, arr = dn.array_to_image (rgb_frame1)
                rgb_frame2 = cv2.cvtColor(img_list [1], cv2.COLOR_BGR2RGB)
                img2, arr = dn.array_to_image (rgb_frame2)

            # start predict
            start_time = time.time ()
            r = dn.detect(net, meta, str.encode(image_path), 0.1)
            
            # directly load img
            r1 = None
            r2 = None
            if do_person:
                r1 = dn.detect_image (net1, meta1, img1, 0.3)
                r2 = dn.detect_image (net1, meta1, img2, 0.3)

            total_time = total_time + time.time () - start_time
            #print(r)

            # load label
            label = []
            if load_label == 1:
                tmp = csv.reader (open (label_path, newline = ''), delimiter = ' ')
                label = list (tmp)

            # load pkl
            label_ = []
            if do_person:
                try:
                    tmp = person_label [file_name + ".jpg"]
                    for t in tmp:
                        x, y = two_to_four ([t[1], t[2]], [t[3], t[4]], img_height)
                        label_.append ([x, y])
                        label_count_ += 1
                except:
                    label_ = []

            outfile = None
            if output_txt == 1:
                outfile = open (result_dir + file_name + ".txt", "a+")
                dic[file_name + ".jpg"] = []

            # draw rectangle (label)
            for t in label:
                if draw_label == 1:
                    x1, y1, x2, y2 = to_xy (t[1:5], img_width, img_height)
                    cv2.rectangle (img, (x1, y1), (x2, y2), (0, 255, 0), 2)
                label_count = label_count + 1

            # draw rectangle (person)
            if draw_label:
                for t in label_:
                    for k in range (4):
                        x1 = int (t [0][k])
                        x2 = int (t [0][(k + 1) % 4])
                        y1 = int (t [1][k])
                        y2 = int (t [1][(k + 1) % 4])
                        cv2.line (img, (x1, y1), (x2, y2), (0, 255, 0), 2)

            # deal prediction
            for j in r:
                count = len (IOU)

                x1 = int (j[2][0] - j[2][2] / 2.0)
                y1 = int (j[2][1] - j[2][3] / 2.0)
                x2 = int (j[2][0] + j[2][2] / 2.0)
                y2 = int (j[2][1] + j[2][3] / 2.0)

                # calculate IOU
                for t in label:
                    if meta.names[int (t[0])] == j[0] and connected ((x1, y1, x2, y2), to_xy (t[1:5], img_width, img_height)):
                        x1_, y1_, x2_, y2_ = to_xy (t[1:5], img_width, img_height)

                        x_min = max (x1, x1_)
                        x_max = min (x2, x2_)
                        y_min = max (y1, y1_)
                        y_max = min (y2, y2_)

                        a = (x2 - x1) * (y2 - y1)
                        b = (x2_ - x1_) * (y2_ - y1_)
                        connect = (x_max - x_min) * (y_max - y_min)

                        iou = float (connect) / float (a + b - connect)

                        IOU.append (iou)
                        percent.append (j[1])
                        iou_file.write ("%f %f\n" % (iou, j[1]))

                # draw rectangle (predict)
                if draw_predict == 1:
                    cv2.rectangle (img, (x1, y1), (x2, y2), (255, 0, 0), 2)
                
                # print class
                if print_info == 1:
                    cv2.putText(img, j[0].decode ("ascii"), (x1 + 2, y1 + 15), cv2.FONT_HERSHEY_DUPLEX,
                                0.6, (0, 0, 0), 1, cv2.LINE_AA)

                # print IOU
                if print_info == 1 and len (IOU) > count:
                    x = int (x2 - 35)
                    y = int (y2 - 15)
                    cv2.rectangle (img, (x, y), (x2, y2), (255, 0, 0), -1)
                    text = int (max (IOU[count:len (IOU)]) * 100)
                    cv2.putText(img, str (text) + "%", (x, y2 - 2), cv2.FONT_HERSHEY_DUPLEX,
                                0.55, (255, 255, 255), 1, cv2.LINE_AA)

                # output txt
                if output_txt:
                    index = 0
                    for k in range (0, meta.classes):
                        if meta.names[k] == j[0]:
                            index = k
                            break

                    outfile.write ("%d %d %d %d %d %.6f\n" % (index, x1, y1, x2, y2, j[1]))
                    dic[file_name + ".jpg"].append ([index, x1, y1, x2, y2, j[1]])

                detect_count = detect_count + 1
                
            # for people (transfer image)
            if do_person:
                r_list, picked = split_combine.combine (416, img_height, [r1, r2])
                resize = img_height / 416
                for j in picked:
                    tmp = r_list [j]

                    x = [int (k * resize) for k in tmp[0]]
                    y = [int (k * resize) for k in tmp[1]]

                    if output_txt:
                        outfile.write ("%d %d %d %d %d %d %d %d %d %.6f\n" % (1, x[0], y[0], x[1], y[1], x[2], y[2], x[3], y[3], tmp[2]))
                        dic[file_name + ".jpg"].append ([1, x[0], y[0], x[1], y[1], x[2], y[2], x[3], y[3], tmp[2]])

                    # draw rectangle
                    if draw_predict:
                        for k in range (4):
                            cv2.line (img, (x[k], y[k]), (x[(k + 1) % 4], y[(k + 1) % 4]), (255, 0, 0), 2)

                    # calculate IOU
                    for t in label_:
                        IOU_tmp = person_IOU ([x, y], t)
                        if IOU_tmp > 0:
                            IOU_person.append (IOU_tmp)
                            percent_person.append (tmp[2])

            if output_txt == 1:
                outfile.close ()

            # image resize
            if img_height < img_width:
                img = cv2.resize (img, (0, 0), fx = img_size / img_width, fy = img_size / img_width)
            else:
                img = cv2.resize (img, (0, 0), fx = img_size / img_height, fy = img_size / img_height)
            
            # store image
            if store_result == 1:
                cv2.imwrite (result_dir + file_name + ".jpg", img)

            # show image
            if show_result == 1:
                cv2.imshow ("image", img)
                cv2.waitKey (1)
                #cv2.destroyAllWindows ()
            
            img_count = img_count + 1

            now = time.time ()
            secs = int (now  - start) % 60
            mins = int ((now  - start) / 60)
            string = ""
            string = string + " %2d:%.2d  %3d %% [" % (mins, secs, int (round (img_count / total * 100, 0)))
            for j in range (int (round (img_count / total * 100, 0))):
                string = string + "="
            for j in range (int (round ((total - img_count) / total * 100, 0))):
                string = string + " "
            string = string + "]"
            string = string + "\r"
            print (string, end = " ")

        if output_txt:
            pickle_out = open (result_dir + "result.pkl", "wb")
            pickle.dump (dic, pickle_out)
            pickle_out.close ()

        tmp = [(IOU[i], percent[i]) for i in range (0, len (IOU))]
        mAP = map.meanAveragePrecision (tmp, label_count)

        tmp = [(IOU_person[i], percent_person[i]) for i in range (0, len (IOU_person))]
        mAP_ = map.meanAveragePrecision (tmp, label_count_)

        print ("\n\n", label_count, " labeled items in ", img_count, " images, ", " ", detect_count, " items detected.")
        print ("Average Precision = ", mAP)
        if len (IOU) > 0:
            print ("IOU highest = ", max (IOU), ", lowest = ", min (IOU), ", average = ", sum (IOU) / len (IOU))
        if do_person:
            print ("Average Precision (person) = ", mAP_)
            if len (IOU_person) > 0:
                print ("IOU (person) highest = ", max (IOU_person), ", lowest = ", min (IOU_person), ", average = ", sum (IOU_person) / len (IOU_person))
        print ("Spend time: ", total_time)

        if write_log == 1:
            outfile = open (log_path, "a+")
            outfile.write ("%d labeled items in %d images, %d items detected.\n" % (label_count, img_count, detect_count))
            outfile.write ("Average Precision = %f\n" % mAP)
            if len (IOU) > 0:
                outfile.write ("IOU highest = %f, lowest = %f, average = %f\n" % (max (IOU), min (IOU), sum (IOU) / len (IOU)))
            outfile.write ("Spend time: %f\n\n" % total_time)
            outfile.close ()

        count_file.write ("%d\n" % label_count)
        iou_file.close ()
        count_file.close ()

    # for video
    if data_type == 1:
        cap = cv2.VideoCapture (video_path)

        if cap.isOpened () == False:
            print ("Fail to read video!")

        video_width = int (cap.get (3))
        video_height = int (cap.get (4))
        video_fps = int (cap.get (cv2.CAP_PROP_FPS))
        total = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
        count = 0
        start = time.time ()

        outfile = None
        if store_result == 1:
            outfile = cv2.VideoWriter (result_dir + "result.mp4", cv2.VideoWriter_fourcc ('m','p','4','v'), video_fps, (video_width, video_height))

        out_txt = None
        if output_txt == 1:
            out_txt = open (result_dir + video_path.split ("/") [-1].split (".") [0] + ".txt", "a+")

        while cap.isOpened ():
            ret, frame = cap.read ()
            frame_num = cap.get(cv2.CAP_PROP_POS_FRAMES)
            #secs = int ((frame_num / video_fps) % 60)
            #mins = int ((frame_num / video_fps) / 60)

            # if read fail then break
            if ret == False:
                break
            
            # transform frame
            rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            img, arr = dn.array_to_image (rgb_frame)
            #---- person frame ----
            img1 = None
            img2 = None
            if do_person:
                img_list = split_combine.one_to_two (video_height, 416, frame)
                rgb_frame1 = cv2.cvtColor(img_list [0], cv2.COLOR_BGR2RGB)
                img1, arr = dn.array_to_image (rgb_frame1)
                rgb_frame2 = cv2.cvtColor(img_list [1], cv2.COLOR_BGR2RGB)
                img2, arr = dn.array_to_image (rgb_frame2)

            # do prediction
            start_time = time.time ()
            r = dn.detect_image (net, meta, img, 0.1)
            r1 = None
            w2 = None
            if do_person:
                r1 = dn.detect_image (net1, meta1, img1, 0.3)
                r2 = dn.detect_image (net1, meta1, img2, 0.3)
            total_time = total_time + time.time () - start_time

            # draw rectangle (predict)
            for i in r:
                x1 = int (i[2][0] - i[2][2] / 2.0)
                y1 = int (i[2][1] - i[2][3] / 2.0)
                x2 = int (i[2][0] + i[2][2] / 2.0)
                y2 = int (i[2][1] + i[2][3] / 2.0)
                cv2.rectangle (frame, (x1, y1), (x2, y2), (255, 0, 0), 2)

                # print class
                if print_info == 1:
                    cv2.putText(frame, i[0].decode ("ascii"), (x1 + 2, y1 + 15), cv2.FONT_HERSHEY_DUPLEX,
                                0.6, (0, 0, 0), 1, cv2.LINE_AA)

                if output_txt == 1:
                    out_txt.write ("%d, %d, %d, %d, %d, %.6f\n" % (int (frame_num), x1, y1, x2, y2, i[1]))

            # draw person 
            if do_person:
                r_list, picked = split_combine.combine (416, video_height, [r1, r2])
                resize = video_height / 416
                for j in picked:
                    tmp = r_list [j]
                    for k in range (4):
                        x1 = int (tmp [0][k] * resize)
                        x2 = int (tmp [0][(k + 1) % 4] * resize)
                        y1 = int (tmp [1][k] * resize)
                        y2 = int (tmp [1][(k + 1) % 4] * resize)
                        cv2.line (frame, (x1, y1), (x2, y2), (255, 0, 0), 2)
                
            # write
            if store_result == 1:
                outfile.write (frame)

            # image resize
            if video_height < video_width:
                frame = cv2.resize (frame, (0, 0), fx = img_size / video_width, fy = img_size / video_width)
            else:
                frame = cv2.resize (frame, (0, 0), fx = img_size / video_height, fy = img_size / video_height)

            # show
            if show_result == 1:
                cv2.imshow ("video", frame)
                if cv2.waitKey (1) != -1:
                    cv2.destroyAllWindows ()
                    break

            now = time.time ()
            secs = int (now  - start) % 60
            mins = int ((now  - start) / 60)
            string = ""
            string = string + " %2d:%.2d  %3d %% [" % (mins, secs, int (round (count / total * 100, 0)))
            for j in range (int (round (count / total * 100, 0))):
                string = string + "="
            for j in range (int (round ((total - count) / total * 100, 0))):
                string = string + " "
            string = string + "]"
            string = string + "\r"
            print (string, end = " ")

            count += 1
        
        # close
        cap.release ()
        if store_result == 1:
            outfile.release ()

        if output_txt == 1:
            out_txt.close ()

        print ("\nFinished!")