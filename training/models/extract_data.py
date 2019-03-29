import glob
from PIL import Image
import numpy as np


def fit_model(data_path, label, X, Y, treshhold):
    for file_path in glob.glob(data_path):
        im = Image.open(file_path)
        X.append(np.array(im))
        Y.append(label)


def fit_model_binary(data_path, label, X, Y):
    for file_path in glob.glob(data_path):
        im = Image.open(file_path)
        grayScale = np.array(im).flatten()
        treshold = grayScale.mean()
        grayScale[grayScale <= treshold] = 1
        grayScale[grayScale > treshold] = 0
        X.append(grayScale)
        Y.append(label)


def fit_model_mean(data_path, label, X, Y):
    for file_path in glob.glob(data_path):
        im = Image.open(file_path)
        X.append([np.array(im).mean()])
        Y.append(label)


def predict_mean(data_path, model, label):
    for file_path in glob.glob(data_path):
        im = Image.open(file_path)
        X = np.array(im).mean()
        print(model.predict([[X]]), label)


def predict_binary(data_path, model, label):
    for file_path in glob.glob(data_path):
        im = Image.open(file_path)
        grayScale = np.array(im).flatten()
        treshold = grayScale.mean()
        grayScale[grayScale <= treshold] = 1
        grayScale[grayScale > treshold] = 0
        print(model.predict([grayScale]), label)


def predict(data_path, model, label):
    for file_path in glob.glob(data_path):
        im = Image.open(file_path)
        X = np.array(im)
        print(model.predict([X]), label)
