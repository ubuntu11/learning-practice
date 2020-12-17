class ScoreCard:

    def __init__(self, score_text: str):
        score_texts = score_text.split('|')
        self.normal_turns = [score_texts[i] for i in range(10)]
        if len(score_texts) == 12:
            self.additional_turns = [score_texts[11]]
        self.all_turns = self.normal_turns + self.additional_turns
        
    def to_score(self):
        sum = 0
        for i in range(len(self.normal_turns)):
            sum = sum + self.get_score_by_turn(i)
        return sum    
            
    def get_score_by_turn(self, turn: int)->int:
        score = self.text_to_score(self.normal_turns[turn])
        if self.__is_strike(self.normal_turns[turn]) or self.__is_spare(self.normal_turns[turn]):
          return score + self.__get_bonus_score(turn)
        else:
          return score
        
    def __get_bonus_score(self, turn:int)->int:
        if turn + 1 == len(self.normal_turns):
            return self.text_to_score(self.additional_turns[0])
        next_2_balls = str(self.all_turns[turn + 1] + self.all_turns[turn + 2])[0:2]     
        return self.text_to_score(next_2_balls)    

    def text_to_score(self, score_text:str)->int:
        if score_text.find('/') == 1:
            return 10

        score = 0
        for i in range(len(score_text)):
            score = score + self.__char_to_score(score_text[i])
        return score    
        
    def __char_to_score(self, score_text:str)->int:
        if self.__is_strike(score_text):
            return 10
        elif  score_text == '-':
            return 0
        else:
            return int(score_text)
            
    def __is_strike(self, score_text: str)->bool:
        return True if score_text.upper() == 'X' else False
        
    def __is_spare(self, score_text: str)->bool:
        return True if score_text.find('/') == 1 else False    
    
