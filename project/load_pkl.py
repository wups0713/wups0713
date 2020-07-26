import pickle

label = pickle.load (open ("result.pkl", "rb"))
label1 = pickle.load (open ("result1.pkl", "rb"))

print (label)
print (label1)