from sklearn.linear_model import LogisticRegression
import glob
from PIL import Image
import numpy
import matplotlib.pyplot as plt
from extract_data import fit_model_binary, fit_model, fit_model_mean, predict_mean, predict_binary, predict


clf_binary = LogisticRegression(random_state=0, solver='lbfgs')
clf_mean = LogisticRegression(random_state=0, solver='lbfgs')

X_Binary = list()
Y_Binary = list()
X_Mean = list()
Y_Mean = list()

print("LR-binary model")
fit_model_binary("./train-data/OK/*.jpg", 0, X_Binary, Y_Binary)
fit_model_binary("./train-data/NOK/*.jpg", 1, X_Binary, Y_Binary)

clf_binary.fit(X_Binary, Y_Binary)

predict_binary("./test-data/OK/*.jpg", clf_binary, 0)
predict_binary("./test-data/NOK/*.jpg", clf_binary, 1)

print(clf_binary.coef_, clf_binary.intercept_)

print("LR-mean model")
fit_model_mean("./train-data/OK/*.jpg", 0, X_Mean, Y_Mean)
fit_model_mean("./train-data/NOK/*.jpg", 1, X_Mean, Y_Mean)

clf_mean.fit(X_Mean, Y_Mean)

predict_mean("./test-data/OK/*.jpg", clf_mean, 0)
predict_mean("./test-data/NOK/*.jpg", clf_mean, 1)
