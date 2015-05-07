#!/usr/bin/env python3

from sklearn.metrics import precision_recall_fscore_support
from sklearn.datasets import load_svmlight_file
from sklearn.svm import LinearSVC
from numpy import unique
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("train", help="training data filename")
parser.add_argument("test", help="testing data filename")

args = parser.parse_args()

X_train, y_train = load_svmlight_file(args.train)
X_test, y_test = load_svmlight_file(args.test)

clf = LinearSVC()
clf.fit(X_train, y_train)
print(clf.score(X_test, y_test))

y_pred = clf.predict(X_test)

print(unique(y_test))
print(unique(y_pred))
print(precision_recall_fscore_support(y_test, y_pred, labels=[-1, 0, +1], average='macro'))
print(precision_recall_fscore_support(y_test, y_pred, labels=[-1, 0, +1], average='micro'))
print(precision_recall_fscore_support(y_test, y_pred, labels=[-1, 0, +1], average='weighted'))
