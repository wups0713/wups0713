import map
import csv

tmp = csv.reader (open ("iou.txt", newline = ''), delimiter = ' ')
index = list (tmp)
tmp = csv.reader (open ("count.txt", newline = ''), delimiter = ' ')
count = list (tmp)

total_count = 0
for i in count:
    total_count += i

ap = map.meanAveragePrecision (index, total_count)
print (ap)