#
# 程式目標見C語言版本註解.
#
rooms = [[False for j in range(3)] for i in range(3)]

room_number = -1
while(room_number != 0):
    room_number = int(input('input room number: '))
    i = int(room_number / 100)
    j = int(room_number - i * 100)
    try:
      rooms[i-1][j-1] = True
    except IndexError:
       print('wrong room number')  

paid, unpaid = 0, 0
for i in range(3):
    for j in range(3):
        if rooms[i][j]:
            paid += 5000
        else:
            unpaid += 5000


print(f'paid= {paid}, unpaid= {unpaid}')
