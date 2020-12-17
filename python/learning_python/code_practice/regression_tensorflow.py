#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Mar 20 21:25:57 2018
使用tensorflow來做簡單線性迴歸的一個例子.
出處：李嘉璇著「用TensorFlow提早進入人工智慧的未來世界」Chapter 8
@author: ken
"""

#%matplotlib inline
import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt

x_data = np.linspace(-1, 1, 300)
x_data = x_data[:, np.newaxis]
noise = np.random.normal(0, 0.05, x_data.shape )
y_data = np.square(x_data) - 0.5 + noise

xs = tf.placeholder(tf.float32, [None, 1])
ys = tf.placeholder(tf.float32, [None, 1])

def add_layer(inputs, in_size, out_size, activation_function = None):
    weights = tf.Variable(tf.random_normal([in_size, out_size]))
    biases = tf.Variable(tf.zeros([1, out_size]) + 0.1 )
    Wx_plus_b = tf.matmul(inputs, weights) + biases
    if activation_function is None:
        outputs = Wx_plus_b
    else:
        outputs = activation_function(Wx_plus_b)
    return outputs

h1 = add_layer(xs, 1, 20, activation_function=tf.nn.relu)
prediction = add_layer(h1, 20 ,1, activation_function=None)

loss = tf.reduce_mean(tf.reduce_sum(tf.square(ys - prediction), reduction_indices=[1]))
train_step = tf.train.GradientDescentOptimizer(0.1).minimize(loss)

init = tf.global_variables_initializer()
sess = tf.Session()
sess.run(init)

# plot the real data
fig = plt.figure()
ax = fig.add_subplot(1,1,1)
ax.scatter(x_data, y_data)
plt.ion()

for i in range(1000):
    sess.run(train_step, feed_dict={xs: x_data, ys: y_data})
    if(i % 50 == 0):
        print (sess.run(loss, feed_dict={xs: x_data, ys: y_data}))

prediction_value = sess.run(prediction, feed_dict={xs: x_data})

# plot the prediction
lines = ax.plot(x_data, prediction_value, 'r-', lw=5)
plt.pause(0.9)