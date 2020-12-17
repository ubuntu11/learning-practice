import hiker
import unittest


class TestHiker(unittest.TestCase):

    def test_life_the_universe_and_everything(self):
        '''simple example to start you off'''
        n_list = hiker.generate_list(3, 7)
        self.assertEqual(4, len(n_list))
        n_list = hiker.generate_list(1, 7)        
        self.assertEqual(6, len(n_list))
        self.assertEqual([1,2,3,4,5,6], n_list)
        
    def test_shuffle_list(self):
        n_list = hiker.shuffle_list(hiker.generate_list(1, 7))
        self.assertEqual(6, len(n_list))       
        self.assertNotEqual([1,2,3,4,5,6], n_list)        


if __name__ == '__main__':
    unittest.main()  # pragma: no cover

