//create an array
String[] names = {"ken", "stefanie", "sabrina", "joy" }
//print out the second item
System.out.println(names[1])
//print out the array langth
System.out.println(names.length)
//convert to a string list and then use its contains method
List<String> namesList = Arrays.asList(names)
System.out.println(namesList.contains("joy"))
//or use java 8 stream api
System.out.println(Arrays.stream(names).anyMatch("joy"::equals))

//exist jshell
/ex
