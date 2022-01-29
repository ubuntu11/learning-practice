cmake_minimum_required(VERSION 3.6) 
project(CHowToCode)

#
# add all c file as executable.
#
#FILE(GLOB MyCSources src/*.c)
#ADD_EXECUTABLE(MyExecutable ${MyCSources})

#
# separate c files into lib and executable
#
set(LIB_SOURCES src/10_18.c src/10_2.c src/10_3.c src/10_7.c src/12_13.c src/12_19.c src/12_3.c src/12_8.c src/7_24.c src/7_26.c src/ex_7_3.c src/ex_8_11.c src/sec_8_5.c)
add_library(libs STATIC "${LIB_SOURCES}")

add_executable(a.out src/main.c)
target_link_libraries(a.out libs)
