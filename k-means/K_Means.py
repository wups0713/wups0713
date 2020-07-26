import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

dataset = pd.read_csv("data_noah.csv")  #read .csv file
df_all = pd.DataFrame(dataset) 
df = df_all[ ['x', 'y'] ]

#print(df['x'])
#plt.scatter(df['x'], df['y'], s = 7)
#plt.show()



# K means
K = 3
max_itr = 5
itr = 0

n_data = np.size(df,axis=0)
n_attr = np.size(df,axis=1)


Group = np.zeros([n_data, K], dtype=float)
Distance = np.zeros([n_data, K], dtype=float)

GroupCenters = df.sample(n=K).reset_index(drop=True)
#n_GroupCenters = np.size(GroupCenters, axis=0)   #equals to K
#print(Centers.values[1])

while itr < 5:
    itr = itr + 1
    for row in range(n_data):
        row_data = df.values[row]

        #Distance = np.zeros(K, dtype=float)
        
        # Calculate the Euclidean distance with k center points
        for c in range(K):
            CenterPoint = GroupCenters.values[c]
            Distance[row, c] = np.linalg.norm(row_data - CenterPoint)  # calculate the euclidean distance (also can use scipy.euclidean)


        
        #print(np.amin(Distance))

        # the center which is closest to the row_data means that
        # the row_data belongs to the group where the center at. 
        

    CenterIndex = np.argmin(Distance, axis=1)  # row_data's group number
    #print(CenterIndex)
    #Group[row, CenterIndex] = row_data

    #print(df.head(5))
    #print(df.values[[1, 2, 4], :])


    for i in range(K):
        print(i)
        new_avg = np.mean(df.values[CenterIndex == i, :], axis=0)
        print(new_avg)
        print(GroupCenters.values[i, :])
        GroupCenters.values[i, :] = new_avg
        
    print('\n')
        #print(df.values[CenterIndex == i, :])
