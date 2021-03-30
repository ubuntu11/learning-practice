/**
 * 簡單的樸克牌遊戲，使用亂數和堆疊洗牌，洗好後發給四家，每家五張牌.
**/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

char* cardTypes[4] = {"space", "heart", "diamond", "club"};
void showCardReading(int cardId) {
  char *cardReading;
  sprintf(cardReading, "%s-%d", cardTypes[cardId / 13], (cardId % 13 + 1));
  printf("%s", cardReading);
}

int main() {
  //堆疊頂端元素的陣列索引
  int top = -1;
  // 洗出來的牌
  int shuffledCards[52];
  // 堆疊內元素
  struct stackItem {
    int cardId; // 0-51，按space,heart,diamond,club順序,(c-1)*13+i
    struct stackItem *next;
  };

  // 記錄各張牌已洗出(1)或未洗出(0)
  int cardIssued[52];
  for (int i=0; i<52; i++) {
    cardIssued[i] = 0;
    shuffledCards[i] = -1;
  }

  srand(time(NULL));
  int cardId = -1;
  while(top < 51) {
    cardId = rand() % 52;
    if (cardIssued[cardId] < 1) {
      cardIssued[cardId] = 1;
      top++;
      shuffledCards[top] = cardId;
    }
  }
  printf("top=%d", top);
  for(int i=0; i<=top; i++) {
    printf("card %d is ", i);
    showCardReading(shuffledCards[i]);
    printf("\n");
  }
}
