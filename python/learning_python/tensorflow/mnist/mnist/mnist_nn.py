# coding=utf-8
"""
用tensorflow範例程式來當做資料來源,自行撰碼來做模型訓練.
"""
from tensorflow.examples.tutorials.mnist import input_data
import tensorflow as tf
import math

# hidden layer 1 with 128 neurons
x1 = tf.placeholder(tf.float32, [None, 784]) # 多列, 784欄
W1 = tf.Variable(tf.truncated_normal([784, 128],  stddev=1.0 / math.sqrt(float(784))))
b1 = tf.Variable(tf.zeros([128]))
y1 = tf.nn.relu(tf.matmul(x1, W1) + b1)

# hidden layer 2 with 32 neurons
x2 = y1 # 多列, 128欄
W2 = tf.Variable(tf.truncated_normal([128, 32],  stddev=1.0 / math.sqrt(float(128))))
b2 = tf.Variable(tf.zeros([32]))
y2 = tf.nn.relu(tf.matmul(x2, W2) + b2)

# output layer with 10 neurons (number 0..9)
x3 = y2 # 多列, 32欄
W3 = tf.Variable(tf.truncated_normal([32, 10],  stddev=1.0 / math.sqrt(float(32))))
b3 = tf.Variable(tf.zeros([10]))
y3 = tf.matmul(x3, W3) + b3

# define loss function
y = tf.placeholder(tf.int64, [None]) # labels
loss = tf.losses.sparse_softmax_cross_entropy(labels=y, logits=y3)

# define training op
optimizer = tf.train.GradientDescentOptimizer(0.01) # learning
training_op = optimizer.minimize(loss)

# create session and initialize all variables
sess = tf.InteractiveSession()
tf.global_variables_initializer().run()

# read data sets and start training
data_sets = input_data.read_data_sets('/tmp/tensorflow/mnist/input_data')
for i in range(5000):
    # batch_xs, batch_ys = 2D and 1D numpy.ndarray
    batch_xs, batch_ys = data_sets.train.next_batch(100)
    sess.run(training_op, feed_dict={x1: batch_xs, y: batch_ys})

# evaluate model
correct_prediction = tf.equal(tf.argmax(input=y3, axis=1), y)
accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
print (sess.run(accuracy, feed_dict={x1: data_sets.test.images, y: data_sets.test.labels}))

