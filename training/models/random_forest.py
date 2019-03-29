from sklearn.ensemble import RandomForestClassifier
import glob
from PIL import Image
import numpy as np
import matplotlib.pyplot as plt
from extract_data import fit_model_binary, fit_model, fit_model_mean, predict_mean, predict_binary, predict

clf_binary = RandomForestClassifier(bootstrap=True, class_weight=None, criterion='gini',
                                    max_depth=2, max_features='auto', max_leaf_nodes=None,
                                    min_impurity_decrease=0.0, min_impurity_split=None,
                                    min_samples_leaf=1, min_samples_split=2,
                                    min_weight_fraction_leaf=0.0, n_estimators=100, n_jobs=None,
                                    oob_score=False, random_state=0, verbose=0, warm_start=False)
clf_mean = RandomForestClassifier(bootstrap=True, class_weight=None, criterion='gini',
                                  max_depth=2, max_features='auto', max_leaf_nodes=None,
                                  min_impurity_decrease=0.0, min_impurity_split=None,
                                  min_samples_leaf=1, min_samples_split=2,
                                  min_weight_fraction_leaf=0.0, n_estimators=100, n_jobs=None,
                                  oob_score=False, random_state=0, verbose=0, warm_start=False)

X_Binary = list()
Y_Binary = list()
X_Mean = list()
Y_Mean = list()

print("RF-binary model")
fit_model_binary("./train-data/OK/*.jpg", 0, X_Binary, Y_Binary)
fit_model_binary("./train-data/NOK/*.jpg", 1, X_Binary, Y_Binary)

clf_binary.fit(X_Binary, Y_Binary)

predict_binary("./test-data/OK/*.jpg", clf_binary, 0)
predict_binary("./test-data/NOK/*.jpg", clf_binary, 1)


print("RF-mean model")
fit_model_mean("./train-data/OK/*.jpg", 0, X_Mean, Y_Mean)
fit_model_mean("./train-data/NOK/*.jpg", 1, X_Mean, Y_Mean)

clf_mean.fit(X_Mean, Y_Mean)

predict_mean("./test-data/OK/*.jpg", clf_mean, 0)
predict_mean("./test-data/NOK/*.jpg", clf_mean, 1)
