# use range to make a for loop
# start from 0
for i in range(10):
    print i

# with starting and ending number
# ending number is excluded
for i in range(1, 10):
    print i

# with step
for i in range(1, 10, 2):
    print i

# loop with list
names = ['ken', 'stefanie', 'sabrina', 'joy']
for name in names:
    print name

# use continue
for i in range(10):
    if i % 3 == 0:
        continue
    print i

# use break
for i in range(1, 10):
    if i % 3 == 0:
       break
    print i

# for with else
for i in range(10):
    if i > 100:
        break
else:
    print "go here if no break!"

# loop by while
a = 0
while a < 10:
    print a
    a = a +1


    """
multiline comment
multiline comment
multiline comment
    """
