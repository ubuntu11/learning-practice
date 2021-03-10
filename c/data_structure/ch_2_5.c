#include <stdio.h>
#include <stdbool.h>

/*
有一棟五層樓出租公寓，房號從101、102、103到501、502、503。每房租金5000元，第201、402、303、102、501、302已交本月租金，
請寫一個程式輸入已交租金的房號，可以顯示未交房租的房號及全部已交、未交的房租總金額。
*/

struct room
{
  int number;
  int paid;
};

void init(struct room rooms[], size_t len) {
  for(int i=0; i < len; i++) {
    struct room a = { (i / 3 + 1) * 100 + (i % 3 + 1), 0};
    rooms[i] = a;
  }
}

int findRoom(int roomNumber, struct room rooms[], size_t len) {
  int idx = -1;
  for(int i=0; i < len; i++) {
    if (rooms[i].number == roomNumber) {
      idx = i;
    }
  }
  return idx;
}

int main()
{
  struct room rooms[15];
  int roomNumber, paidRoomNumber;
  int count = 0;
  int totalPaid = 0, totalUnpaid = 0;

  init(rooms, 15);
  while(paidRoomNumber != 0 && count < 15) {
    puts("input paid room number: (input 0 when finish)");
    scanf("%d", &paidRoomNumber);
    if (paidRoomNumber != 0) {
      printf("room number is %d", roomNumber);
      roomNumber = findRoom(paidRoomNumber, rooms, 15);
      if (roomNumber > 0) rooms[roomNumber].paid = 1;
      count ++;
    }
  }

  for(int i=0; i<15; i++) {
    if (rooms[i].paid == 0) {
      totalUnpaid += 5000;
      printf("room(%d) not paid yet.\n", rooms[i].number);
    } else {
      totalPaid += 5000;
    }
  }
  printf("total paid money=%d, unpaid money=%d\n", totalPaid, totalUnpaid);
}
