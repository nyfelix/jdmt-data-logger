from sklearn import linear_model
import numpy
from extract_data import convert_to_binary_image
from data import X_NOK, X_OK
from sklearn.model_selection import LeaveOneOut
from extracted_lr_model import predict, predict_with_betas_simplified
from PIL import Image


def predict_all(X, Y):
    clf = linear_model.LogisticRegression(
        C=1e5, solver='lbfgs', max_iter=10000, random_state=12)
    loo = LeaveOneOut()
    first = True
    for train_index, test_index in loo.split(X):
        X_train, X_test = X[train_index], X[test_index]
        y_train, y_test = Y[train_index], Y[test_index]
        clf.fit(X_train, y_train)
        prediction = clf.predict(X_test)[0]
        if(first):
            _, length = clf.coef_.shape
            print("nof coef:", length)
            first = False
        if(prediction != y_test[0]):
            print("wrong prediction",
                  prediction, "true value", y_test[0])


img = Image.fromarray(X_OK[5], 'L')
img.show()

X = numpy.concatenate((X_NOK, X_OK), axis=0)
X = convert_to_binary_image(X[:])
print(X[0])
nofRows, rowSize, colSize = X.shape
X = numpy.reshape(X, [nofRows, rowSize*colSize])
Y = numpy.concatenate((numpy.zeros([20]), numpy.ones([59])))
print("prediction with full model. testsize:", colSize)
predict_all(X, Y)

X = numpy.concatenate((X_NOK, X_OK), axis=0)
X = X.mean(axis=1)
nofRows, rowSize = X.shape
Y = numpy.concatenate((numpy.zeros([20]), numpy.ones([59])))
print("prediction with simplified model. testsize:", colSize)
predict_all(X, Y)

# for index in range(nofRows):
#  print(predict_with_betas_simplified(X[index]), "==", Y[index])
