from sklearn.neural_network import MLPRegressor
from sklearn.neural_network import BernoulliRBM
from sklearn.utils import shuffle
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split

import tensorflow as tf

df = pd.read_csv('out_sf_depth_4.csv')
X = df.iloc[:, 1:].to_numpy()
y = df.iloc[:, 0].to_numpy()
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.20, random_state=1)

km = tf.keras.models.Sequential()
km.add(tf.keras.layers.Dense(1000, input_shape=(748,), activation='tanh'))
km.add(tf.keras.layers.Dense(500, activation='tanh'))
km.add(tf.keras.layers.Dense(250, activation='tanh'))
km.add(tf.keras.layers.Dense(125, activation='tanh'))
km.add(tf.keras.layers.Dense(60, activation='tanh'))
km.add(tf.keras.layers.Dense(10, activation='tanh'))
km.add(tf.keras.layers.Dense(1, activation='tanh'))
km.compile(optimizer='rmsprop', loss='mse')
m = km.fit(X_train, y_train, epochs=100)
y_test_hat = np.transpose(km.predict(X_test))
np.mean((y_test_hat - y_test) ** 2)
np.corrcoef(y_test_hat, y_test)

reg = MLPRegressor(activation='relu', hidden_layer_sizes=(10, 10, 10, 10, 10), verbose=True, n_iter_no_change=1000,
                   max_iter=10000, beta_1=0, shuffle=True, random_state=1, early_stopping=True)
reg.out_activation_ = 'tanh'
model = reg.fit(X_train, y_train)

rbm = BernoulliRBM(n_components=200, verbose=True)
rbmm_odel = rbm.fit(X_train)

y_test_hat = model.predict(X_test)
np.corrcoef(y_test_hat, y_test)
plt.plot(y_test, y_test_hat, '*')
plt.show()
