import numpy as np
import pandas as pd
import tensorflow as tf
import re
from matplotlib import pyplot as plt

df = pd.read_csv('train.csv', header=None)
X = df.iloc[:, 1:].to_numpy().astype('float64')
y = df.iloc[:, 0].to_numpy().astype('float64')
# X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.10, random_state=1)

km = tf.keras.models.Sequential()
km.add(tf.keras.layers.Dense(1, input_shape=(748,), activation='tanh'))
# km.add(tf.keras.layers.Dense(1, activation='tanh'))

km.compile(optimizer='sgd', loss='mse')

with open('weights.txt') as weights_file:
    layers = int(weights_file.readline())
    for layer in range(layers):
        rows, cols = [int(x) for x in weights_file.readline().split()]
        matrix = np.zeros([rows, cols])
        for row in range(rows):
            line = weights_file.readline()
            matrix[row, :] = np.fromstring(line, sep=' ', dtype='float64')
        km.weights[2 * layer].assign(matrix[:, 1:].T)
        km.weights[2 * layer + 1].assign(matrix[:, 0].T)

m = km.fit(X, y, epochs=1)

with open('weights.new.txt', 'w') as weights_file:
    layers = len(km.layers)
    weights_file.write("%d\n" % layers)
    for layer in range(layers):
        weights = km.layers[layer].weights[0].numpy().T
        bias = km.layers[layer].weights[1].numpy().T
        matrix = np.hstack([bias.reshape(weights.shape[0], 1), weights])
        weights_file.write("%d %d\n" % matrix.shape)
        weights_file.write(np.array2string(matrix, max_line_width=1000000, separator=' ')
                           .replace('[', '')
                           .replace(']', ''))


plt.bar(x=np.arange(748), height=km.weights[0][:, 0])
plt.savefig('weights.png')
