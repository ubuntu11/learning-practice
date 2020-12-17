import tensorflow as tf
import numpy as np

a = tf.constant([[-1.0, 2.0, 3.0, 4.0]])
sess = tf.Session()
b = tf.nn.dropout(a, 0.5, noise_shape=[1, 4])
print(sess.run(b))