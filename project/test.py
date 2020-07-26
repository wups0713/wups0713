import math
import shapely
import numpy as np
from shapely.geometry import Polygon,MultiPoint

a = [620, 722]                          # point1
b = [549, 917]                        # point2
x = [0, 1, 0, -1]                   # x list from combine
y = [1, 0, -1, 0]                   # y list from combine
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

print ([a, a1, b, b1])
poly1 = Polygon ([a, a1, b, b1]).convex_hull
poly2 = Polygon ([[x[0], y[0]], [x[1], y[1]], [x[2], y[2]], [x[3], y[3]]]).convex_hull

if poly1.intersects (poly2):
    inter_area = poly1.intersection (poly2).area
    print (float (inter_area) / (poly1.area + poly2.area - inter_area))
else:
    print (0)