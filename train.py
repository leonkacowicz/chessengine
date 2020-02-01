import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
import boto3
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
m = km.fit(X_train, y_train, epochs=1)
y_test_hat = np.transpose(km.predict(X_test))
np.mean((y_test_hat - y_test) ** 2)
np.corrcoef(y_test_hat, y_test)