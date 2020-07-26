import numpy as np
from sklearn import tree
from sklearn.datasets import load_iris
import pydot	# It requires installing pydot and graphviz
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.utils import shuffle
from sklearn.metrics import confusion_matrix
from sklearn.metrics import accuracy_score
from sklearn.metrics import classification_report
import random

#read file, initial
iris = pd.read_csv ('iris.csv')     #read csv
df = pd.DataFrame (iris)            #build dataframe
data = list(df.columns.values)[0:4]
target = list(df.columns.values)[4]
iris_target_names = ['Iris-setosa', 'Iris-versicolor', 'Iris-virginica']
num_of_trees = 5
num_of_datas = df.count()[0]
bag_size = num_of_datas / num_of_trees

#preprocessing
#df.fillna(df.mean()['B':'C']) fill in nan with average (for some columns)   preprocess

#draw
#df[data].plot.hist(alpha = 0.5, normed = True)                 #histogram
#group = df[data].groupby(df[target], as_index = False).mean()
#group.index = iris_target_names
#group.plot.bar()                                               #bar
#plt.show()                                                     #show all plots

#random forest
accu_vec = []
precision_vec = []
recall_vec = []
matrix_vec = []
test = []
Result = []

K = 10
#　k-fold vallidation
df_shuffle = df.copy()
df_shuffle = shuffle(df_shuffle).reset_index(drop = True)
for z in range(K):
    test = []
    trees  = []
    result = []
    
    n_test = int(num_of_datas / K)
    train = df_shuffle.copy()
    Max_tmp = (z+1)*n_test - 1
    if z == K - 1 :
        Max_tmp = num_of_datas
    test  = train.loc[(z*n_test) : Max_tmp].reset_index(drop=True)
    if K != 1 : 
        train.drop(train.index[(z*n_test) : Max_tmp], inplace = True)        #drop some rows

    for i in range(num_of_trees):                                   #building trees
        train = shuffle(train).reset_index(drop = True)              #random
        random_col = shuffle(data)[0:random.randint(2, 4)]                                  #get random columns
        #print(random_col)

        clf = tree.DecisionTreeClassifier()
        clf = clf.fit(train.loc[0:30, random_col], train.loc[0:30, target])
        trees.append([clf, random_col])

    #feeding testing data 
    for i in range(int(test.count()[0])):                        
        vote = [0, 0, 0]
        for j in range(num_of_trees):
            r = trees[j][0].predict([ test.loc[i,trees[j][1]] ])   #trees[j][1] == random_col
            index = iris_target_names.index(r)
            vote[index] = vote[index] + 1
        result.append(iris_target_names[vote.index(max(vote))])
        Result.append(iris_target_names[vote.index(max(vote))])

    #output result
    matrix_vec.append(confusion_matrix(test[target], result, labels = iris_target_names))

    statistic = classification_report(test[target], result, target_names=iris_target_names, output_dict=True)
    tmp_precision = []
    tmp_recall = []
    for species in iris_target_names:
        tmp_precision.append(statistic[species]['precision'])
        tmp_recall.append(statistic[species]['recall'])
    precision_vec.append(tmp_precision)
    recall_vec.append(tmp_recall)

    accu_vec.append(accuracy_score(test[target], result))

#print confusion matrix
ConfusionMatrix = sum(matrix_vec)
print('\nconfusion matrix : \n', ConfusionMatrix, '\n')

#compute precision and recall
statistic = classification_report(df_shuffle[target], Result, target_names=iris_target_names, output_dict=True)

#print precision and recall
sum_precision = [sum(i) for i in zip(*precision_vec)]
precision_array = np.array(sum_precision)
precision_array = precision_array / K

sum_recall = [sum(i) for i in zip(*recall_vec)]
recall_array = np.array(sum_recall)
recall_array = recall_array / K

output_df = pd.DataFrame(index = iris_target_names)
output_df['precision'] = precision_array
output_df['recall'] = recall_array
print(output_df)

#print accuracy
accu_mean = sum(accu_vec) / len(accu_vec)
print('\nTotal accuracy :', accu_mean)