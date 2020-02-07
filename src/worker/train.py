import numpy as np
import pandas as pd
import tensorflow as tf

df = pd.read_csv('train.csv', header=None)
X = df.iloc[:, 1:].to_numpy()
y = df.iloc[:, 0].to_numpy()
# X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.10, random_state=1)

km = tf.keras.models.Sequential()
km.add(tf.keras.layers.Dense(100, input_shape=(748,), activation='tanh'))
km.add(tf.keras.layers.Dense(1, activation='tanh'))
km.compile(optimizer='rmsprop', loss='mse')

with open('weights.txt') as weights_file:
    layers = int(weights_file.readline())
    for layer in range(layers):
        rows, cols = [int(x) for x in weights_file.readline().split()]
        matrix = np.zeros([rows, cols])
        for row in range(rows):
            matrix[row, :] = np.asarray([float(x) for x in weights_file.readline().split()])
        km.weights[2 * layer].assign(matrix[:, 1:].T)
        km.weights[2 * layer + 1].assign(matrix[:, 0].T)

m = km.fit(X, y, epochs=150)

with open('weights.new.txt', 'w') as weights_file:
    layers = len(km.layers)
    weights_file.write("%d\n" % layers)
    for layer in range(layers):
        [rows, cols] = km.weights[layer * 2].numpy().T.shape
        weights_file.write("%d %d\n" % (rows, cols + 1))
        for row in range(rows):
            line = "%.8f " % km.weights[2 * layer + 1][row]
            line += np.array2string(
                km.weights[layer * 2].numpy().T[row], precision=8, separator=' ')[2:-1].replace('\n', ' ') + '\n'
            weights_file.write(line)
