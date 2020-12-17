#
# 2018-07-07 Ken Hu
# Use tensorflow api directly to do ann.py (ML A-Z的範例程式，使用keras來訓練神經網路)
#


# Part 1 - Data Preprocessing，這個部份與使用keras的版本是完全一樣的

# Importing the libraries
import pandas as pd
from common_functions import iterate_minibatches

# Importing the dataset
from tensorflow import Variable

dataset = pd.read_csv('Churn_Modelling.csv')
X = dataset.iloc[:, 3:13].values
y = dataset.iloc[:, 13].values
y = y.reshape(y.size, -1)

# 處理分類資料，國家與性別
from sklearn.preprocessing import LabelEncoder, OneHotEncoder
labelencoder_X_1 = LabelEncoder()
X[:, 1] = labelencoder_X_1.fit_transform(X[:, 1])
labelencoder_X_2 = LabelEncoder()
X[:, 2] = labelencoder_X_2.fit_transform(X[:, 2])
onehotencoder = OneHotEncoder(categorical_features = [1])
X = onehotencoder.fit_transform(X).toarray()
X = X[:, 1:]

# 把資料成為訓練集與測試集
from sklearn.model_selection import train_test_split
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size = 0.2, random_state = 0)

# 特徵縮放 Feature Scaling
from sklearn.preprocessing import StandardScaler
sc = StandardScaler()
X_train = sc.fit_transform(X_train)
X_test = sc.transform(X_test)

# Part 2 - 建立人工神經網路
import tensorflow as tf

# define tf.placeholders
xs = tf.placeholder(tf.float32, [None, 11])
ys = tf.placeholder(tf.float32, [None, 1])

# 1st hidden layer(h1), use relu as activation func
# define tf.variables that will be adjuseted during trainings
weights_h1 = tf.Variable(tf.random_normal([11, 6]))
biases_h1 = tf.Variable(tf.zeros([1, 6]) + 0.1)
wx_plus_b_h1 = tf.nn.relu(tf.matmul(xs, weights_h1) + biases_h1)

# 2nd hidden layer(h2)
# define tf.variables that will be adjuseted during trainings
weights_h2 = tf.Variable(tf.random_normal([6, 6]))
biases_h2 = tf.Variable(tf.zeros([1, 6]) + 0.1)
wx_plus_b_h2 = tf.nn.relu(tf.matmul(wx_plus_b_h1, weights_h2) + biases_h2)

# output layer(ol), use sigmoid function (output a floating number btn 0~1)
weights_ol = tf.Variable(tf.random_normal([6, 1]))
biases_ol: Variable = tf.Variable(tf.zeros([1, 1]) + 0.1)
wx_plus_b_ol = tf.nn.sigmoid(tf.matmul(wx_plus_b_h2, weights_ol) + biases_ol)

# define loss function：算出來的是一個數字，所以用不到cross entropy這種東西，用簡單的均方误差（mean squared error，各数据偏离真实
# 值的距离平方和的平均数）即可
loss = tf.reduce_mean(tf.reduce_sum(tf.square(ys - wx_plus_b_ol), reduction_indices=[1]))

# define training step，所謂的訓練，就是讓指定的優化器（optimizer）最小化損失函數
training = tf.train.GradientDescentOptimizer(0.1).minimize(loss)

# define evaluation method
# 用tf.round對預測出來的數字做四捨五入（假設>=0.5就會離開這家銀行），然後和已知標籤（0或1）做比較
correct_prediction = tf.equal(tf.round(wx_plus_b_ol), ys)
accuracy = tf.reduce_mean(tf.cast(correct_prediction, "float")) # Calculate accuracy

# initialize variables
init = tf.global_variables_initializer()
epoch_number = 10000
batch_size = 100

with tf.Session() as sess:
    sess.run(init)

    for i in range(epoch_number):
        for batch in iterate_minibatches(X_train, y_train, batch_size, shuffle=True):
            X_batch, y_batch = batch
            sess.run(training, feed_dict={xs: X_batch, ys: y_batch})
        if i % 10 == 0:
            print(sess.run(loss, feed_dict={xs: X_train, ys: y_train}))
    # 把測試集送往神經網路，並列印出wx_plus_b_ol這個Tensor的值
    print(wx_plus_b_ol.eval({xs: X_test, ys: y_test}))
    print("Accuracy:", accuracy.eval({xs: X_test, ys: y_test}))
