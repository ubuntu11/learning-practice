import unittest
import os
from mnist.mnist_dataset import DataSet

class TestMnistDataSet(unittest.TestCase):
    def setUp(self):
        #test.csv只有1000筆樣本
        self.dataset = DataSet(os.path.join(os.path.dirname(__file__), 'test.csv'))
        
    def test_dataset_shape(self):
        self.assertTrue(True)
        self.assertEqual(1000, self.dataset.images.shape[0])
        self.assertEqual(784, self.dataset.images.shape[1])
        
    def test_next_batch(self):
        images, labels = self.dataset.next_batch(300)
        self.assertEqual(300, (images.shape[0]))
        images, labels = self.dataset.next_batch(300)
        self.assertEqual(300, (images.shape[0]))
        images, labels = self.dataset.next_batch(300)
        self.assertEqual(300, (images.shape[0]))
        images, labels = self.dataset.next_batch(300)
        self.assertEqual(300, (images.shape[0]))

if __name__ == '__main__':
    unittest.main()
