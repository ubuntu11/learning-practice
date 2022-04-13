QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    emslink.cpp \
    main.cpp \
    mainwindow.cpp \
    mbmslink.cpp \
    meterlink.cpp \
    pcslink.cpp

HEADERS += \
    emslink.h \
    mainwindow.h \
    mbmslink.h \
    meterlink.h \
    pcslink.h

FORMS += \
    emslink.ui \
    mainwindow.ui \
    mbmslink.ui \
    meterlink.ui \
    pcslink.ui
INCLUDEPATH += C:\Users\orang\eclipse-workspace\AFC_GC
INCLUDEPATH += C:\Users\orang\eclipse-workspace\system_test
#INCLUDEPATH += C:\msys64\mingw64\local\include\bsoncxx\v_noabi
#INCLUDEPATH += C:\msys64\mingw64\local\include\mongocxx\v_noabi

#LIBS += -LC:\Users\XU\Documents\AFC_GC -lAFC_GC -lbsoncxx -lsystem_test -lmongocxx
LIBS += -LC:\Users\orang\eclipse-workspace\AFC_GC\Debug -lAFC_GC
LIBS += -LC:\Users\orang\eclipse-workspace\system_test\Debug -lsystem_test
LIBS += -LC:\msys64\mingw64\bin -lbsoncxx -lmongocxx #-lmongoc-1.0 -lbson-1.0
LIBS += -lconfig++
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
DISTFILES += \
    libAFC_GC.dll \
    libbsoncxx.dll \
    libmongocxx.dll \
    libsystem_test.dll
