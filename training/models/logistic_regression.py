from sklearn import linear_model
import numpy
from extract_data import convert_to_binary_image
from data import X_NOK, X_OK
from extracted_lr_model import predict, predictWithBetas


X = numpy.concatenate((X_NOK, X_OK), axis=0)
X = convert_to_binary_image(X[:])
nofRows, rowSize, colSize = X.shape
X = numpy.reshape(X, [nofRows, rowSize*colSize])
Y = numpy.concatenate((numpy.zeros([20]), numpy.ones([59])))


clf = linear_model.LogisticRegression(C=1e5, solver='lbfgs')
clf.fit(X, Y)

for index in range(nofRows):
    print(predictWithBetas(X[index]), "==", Y[index])
