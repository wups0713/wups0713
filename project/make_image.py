import cv2
import os
import time

frames = input ("Frames: ")

if os.path.isdir ("result") == False:
    os.mkdir ("result")

while 1:
    video_path = input ("Input file path: ")

    file_name = video_path.split ("/") [-1].split (".") [0]

    cap = cv2.VideoCapture (video_path)
    total = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    count = 0
    start = time.time ()

    while cap.isOpened ():
        ret, frame = cap.read ()
        count += 1

        if ret == False:
            break

        if count % int (frames) == 1:
            #cv2.imshow ("frame", frame)

            #if cv2.waitKey(0) & 0xFF == ord('y'):
            cv2.imwrite ("result/%s_%d.jpg" % (file_name, int (count / int (frames))), frame)

            string = ""
            now = time.time ()
            secs = int (now  - start) % 60
            mins = int ((now  - start) / 60)
            string = string + " %2d:%.2d  %3d %% [" % (mins, secs, int (round (count / total * 100, 0)))
            for j in range (int (round (count / total * 100, 0))):
                string = string + "="
            for j in range (int (round ((total - count) / total * 100, 0))):
                string = string + " "
            string = string + "]\r"
            print (string, end = '')

    now = time.time ()
    secs = int (now  - start) % 60
    mins = int ((now  - start) / 60)
    print (" %2d:%.2d  100 %% [====================================================================================================\nFinished!" % (mins, secs))

    cap.release ()