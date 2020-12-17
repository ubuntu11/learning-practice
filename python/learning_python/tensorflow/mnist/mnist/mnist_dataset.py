#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jun 13 19:11:23 2018

@author: ken
see /Users/ken/Documents/tensorflow-1.6.0/tensorflow/contrib/learn/python/learn/datasets/mnist_core.py
"""
import pandas as pd
import numpy as np

IMAGE_SIZE = 28
IMAGE_PIXELS = IMAGE_SIZE * IMAGE_SIZE

class DataSet(object):
    def __init__(self, csv_file):
        df = pd.read_csv(csv_file)
        # convert pandas.dataframe to numpy.ndarray use values attribute
        self._images = df.iloc[:, 1:-1].values
        #self._labels = dense_to_one_hot(df.iloc[:, 0].values, 10) #one_hot = True
        self._labels = df.iloc[:, 0].values
        self._index_in_epoch = 0
        self._num_examples = self._images.shape[0]
        self._epochs_completed = 0

    @property
    def images(self):
        return self._images

    @property
    def labels(self):
        return self._labels

    @property
    def num_examples(self):
        return self._num_examples
        
    def next_batch(self, batch_size):
        """Return the next `batch_size` examples from this data set as 2D numpy ndarray."""
        start = self._index_in_epoch
        
        # Shuffle for the first epoch(每個epoch是要用多個batch把所有樣本都送入網路訓練一次)
        if self._epochs_completed == 0 and start == 0:
            perm0 = np.arange(self._num_examples)
            np.random.shuffle(perm0)
            self._images = self.images[perm0]
            self._labels = self.labels[perm0]

        #進入下一個epoch
        if start + batch_size > self._num_examples:
            # 代表這次 epoch 完成
            self._epochs_completed += 1
            # 這次 epoch 到目前為止剩餘的樣本
            rest_num_examples = self._num_examples - start
            images_rest_part = self._images[start:self._num_examples]
            labels_rest_part = self._labels[start:self._num_examples]
            
            # Shuffle the data
            perm = np.arange(self._num_examples)
            np.random.shuffle(perm)
            self._images = self.images[perm]
            self._labels = self.labels[perm]

            # Start next epoch
            start = 0
            self._index_in_epoch = batch_size - rest_num_examples
            end = self._index_in_epoch
            images_new_part = self._images[start:end]
            labels_new_part = self._labels[start:end]
            return np.concatenate((images_rest_part, images_new_part), axis=0), np.concatenate((labels_rest_part, labels_new_part), axis=0)
        else:
            self._index_in_epoch += batch_size
            end = self._index_in_epoch
            return self._images[start:end], self._labels[start:end]

def dense_to_one_hot(labels_dense, num_classes):
  """Convert class labels from scalars to one-hot vectors."""
  num_labels = labels_dense.shape[0]
  index_offset = np.arange(num_labels) * num_classes
  labels_one_hot = np.zeros((num_labels, num_classes))
  labels_one_hot.flat[index_offset + labels_dense.ravel()] = 1
  return labels_one_hot

def main():
    #data = DataSet('/Users/ken/IdeaProjects/MyJavaDojo/mnist-training-set.csv')
    labels_one_hot = dense_to_one_hot(np.array([0,1,2,3,4,5]),10)
    print(labels_one_hot)
    
        
if __name__ == '__main__':
    main()        