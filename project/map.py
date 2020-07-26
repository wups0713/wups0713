def sortSecond(val): 
    return val[1] 

def meanAveragePrecision(IOU, n):
    IOU.sort(key=sortSecond)
    TruePostive, precision, recall, ex_recall, area, i =  0.0, 0.0, 0.0, 0.0, 0.0, 0
    while i < len(IOU):
        if IOU[i][0] >= 0.5:
            TruePostive = TruePostive + 1
        precision = TruePostive / (i + 1)
        recall = TruePostive / n
        if recall > ex_recall:
            j = i
            tmp_tp, tmp_pre, tmp_rec, max_pre, to_rec = TruePostive-1, 0.0, 0.0, 0.0, 0.0
            while j < len(IOU):
                if IOU[j][0] >= 0.5:
                    tmp_tp = tmp_tp + 1
                tmp_pre = tmp_tp / (j + 1)
                tmp_rec = tmp_tp / n
                if max_pre <= tmp_pre:
                    max_pre = tmp_pre
                    to_rec = tmp_rec
                    i = j
                j = j + 1
            precision = max_pre
            recall = to_rec
            area = area + precision * (recall - ex_recall)
            ex_recall = recall
        i = i + 1
    return area