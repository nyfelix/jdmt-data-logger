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

print(predict(X[0], clf), Y[0])
print(predict(X[1], clf), Y[1])
print(predict(X[3], clf), Y[3])

print(predict(X[21], clf), Y[21])
print(predict(X[22], clf), Y[22])
print(predict(X[50], clf), Y[50])
print("extracted...")
print(predictWithBetas(X[0]), Y[0])
print(predictWithBetas(X[1]), Y[1])
print(predictWithBetas(X[3]), Y[3])

print(predictWithBetas(X[21]), Y[21])
print(predictWithBetas(X[22]), Y[22])
print(predictWithBetas(X[50]), Y[50])
