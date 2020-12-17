# coding: utf-8

import tensorflow as tf
import pandas as pd
import numpy as np
from mnist import mnist_dataset

# load data set
trainingSet = mnist_dataset.DataSet('/Users/ken/Development/java/MyJavaDojo/mnist-training-set.csv')
testSet = mnist_dataset.DataSet('/Users/ken/Development/java/MyJavaDojo/mnist-test-set.csv')

# Create the model
x = tf.placeholder(tf.float32, [None, 784])
W = tf.Variable(tf.zeros([784, 10]))
b = tf.Variable(tf.zeros([10]))
y = tf.matmul(x, W) + b

# Define loss and optimizer
y_ = tf.placeholder(tf.int64, [None]) # one_hot = False
# y_ = tf.placeholder(tf.float32, [None, 10]) # one_hot = True

# define cross entropy and traing step
cross_entropy = tf.losses.sparse_softmax_cross_entropy(labels=y_, logits=y) # one_hot = False
# cross_entropy = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(labels=y_, logits=y)) # one_hot = True
train_step = tf.train.GradientDescentOptimizer(0.5).minimize(cross_entropy)

sess = tf.InteractiveSession()
tf.global_variables_initializer().run()

for i in range(1000):
    batch_xs, batch_ys = trainingSet.next_batch(100)
    sess.run(train_step, feed_dict={x: batch_xs, y_: batch_ys})

correct_prediction = tf.equal(tf.argmax(y, 1), y_) # one_hot = False # one_hot = False
# correct_prediction = tf.equal(tf.argmax(y, 1),tf.argmax(y_, 1)) # one_hot = True # one_hot = True

accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
print (sess.run(accuracy,
                feed_dict={x: testSet.images, y_: testSet.labels}
         ))




