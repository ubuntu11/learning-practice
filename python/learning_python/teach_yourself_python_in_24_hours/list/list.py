#create a list
names = ['ken','stefanie','sabrina', 'joy']
#get the second item
print "second item is: " + names[1]
#show list length
print "list lenth is: " + str(len(names))
#does this list contain someting?
print "does someone named joy? " + str("joy" in names)
#where is sabrina?
print "where is sabrina ? " + str(names.index("sabrina"))
#add a new member
names.append("kuro")
print "list size after adding a new member: " + str(len(names))
#merge with another list
toy_names = ['bear', 'drum']
names.extend(toy_names)
print "list size after merging another list: " + str(len(names))
#remove an item
names.remove("bear")
print "list items after removing bear: " + str(names)
#insert an new item in between
names.insert(3, "car")
print "list items after inserting a new item: " + str(names)
#list can add another list
print "list add by itself: " + str(names + names)
#list can multiply with an integer
print "list multiply with an integer: " + str(names * 3)
#reverse list items
_tmp = names
_tmp.reverse()
print "reverse items: " + str(_tmp)
#sort list items
_tmp = names
_tmp.sort()
print "sort items: " + str(_tmp)
#compare 2 lists
_tmp = []
_tmp.extend(names)
_tmp.reverse()
print "compare 2 list: " + str(_tmp == names)
