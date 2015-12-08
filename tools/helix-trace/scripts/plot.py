#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np
import argparse

def fill_missing(data):
    mask = np.isnan(data)
    data[mask] = np.interp(np.flatnonzero(mask), np.flatnonzero(~mask), data[~mask])

parser = argparse.ArgumentParser()
parser.add_argument("input", help="input filename")
args = parser.parse_args()

data = np.genfromtxt(args.input, delimiter=',', names=True)
fig = plt.figure()
ax = fig.add_subplot(111)

last = data['LastPrice']

vwap = data['VWAP']
fill_missing(vwap)

bid = data['BidPrice']
fill_missing(bid)

ask = data['AskPrice']
fill_missing(ask)

ax.plot(data['Timestamp'], last, 'ko')
ax.plot(data['Timestamp'], bid,  'g-')
ax.plot(data['Timestamp'], ask,  'r-')
ax.plot(data['Timestamp'], vwap, 'y-')
plt.show()
