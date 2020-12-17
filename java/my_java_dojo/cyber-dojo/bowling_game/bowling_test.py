import hiker
import unittest


class TestHiker(unittest.TestCase):

    def test_all_strikes(self):
        '''simple example to start you off'''
        sc = hiker.ScoreCard('X|X|X|X|X|X|X|X|X|X||XX')
        self.assertEqual(10, len(sc.normal_turns))        
        self.assertEqual(1, len(sc.additional_turns))
        self.assertEqual(300, sc.to_score())           
        
    def test_last_strike(self):
        '''simple example to start you off'''
        sc = hiker.ScoreCard('X|27|X|X|72|X|X|81|8-|X||72')
        self.assertEqual(10, len(sc.normal_turns))
        self.assertEqual(1, len(sc.additional_turns)) 
        self.assertEqual(166, sc.to_score())        
        
    def test_last_spare(self):
        '''simple example to start you off'''
        sc = hiker.ScoreCard('X|27|X|X|72|X|X|81|8-|7/||9')
        self.assertEqual(10, len(sc.normal_turns))
        self.assertEqual(1, len(sc.additional_turns)) 
        self.assertEqual(166, sc.to_score())        
        
    def test_text_to_score(self):
        sc = hiker.ScoreCard('X|27|X|X|72|X|X|81|8-|7/||9')    
        self.assertEqual(10, sc.text_to_score(sc.normal_turns[0]))
        self.assertEqual(9, sc.text_to_score(sc.normal_turns[1]))
        self.assertEqual(10, sc.text_to_score(sc.normal_turns[2]))
        self.assertEqual(10, sc.text_to_score(sc.normal_turns[3]))
        self.assertEqual(9, sc.text_to_score(sc.normal_turns[4]))
        self.assertEqual(10, sc.text_to_score(sc.normal_turns[5]))
        self.assertEqual(10, sc.text_to_score(sc.normal_turns[6]))
        self.assertEqual(9, sc.text_to_score(sc.normal_turns[7]))
        self.assertEqual(8, sc.text_to_score(sc.normal_turns[8]))
        self.assertEqual(10, sc.text_to_score(sc.normal_turns[9]))        
        
    def test_get_score_by_turn(self):
        sc = hiker.ScoreCard('X|27|X|X|72|X|X|81|8-|7/||9')    
        self.assertEqual(19, sc.get_score_by_turn(0))
        self.assertEqual( 9, sc.get_score_by_turn(1))
        self.assertEqual(27, sc.get_score_by_turn(2))
        self.assertEqual(19, sc.get_score_by_turn(3))
        self.assertEqual( 9, sc.get_score_by_turn(4))       
        self.assertEqual(28, sc.get_score_by_turn(5))
        self.assertEqual(19, sc.get_score_by_turn(6))
        self.assertEqual( 9, sc.get_score_by_turn(7))
        self.assertEqual( 8, sc.get_score_by_turn(8))
        self.assertEqual(19, sc.get_score_by_turn(9))                
        


if __name__ == '__main__':
    unittest.main()  # pragma: no cover

