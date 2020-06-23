QT += core
QT += network
QT += sql
QT -= gui

CONFIG += c++14 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        newclient.cpp \
        newserver.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    newclient.h \
    newserver.h

win32: LIBS += -lWS2_32

win32: LIBS += -LC:/local2/emiplib/lib/ -lemiplib

INCLUDEPATH += C:/local2/emiplib/include
DEPENDPATH += C:/local2/emiplib/include


win32:!win32-g++: PRE_TARGETDEPS += C:/local2/emiplib/lib/emiplib.lib
else:win32-g++: PRE_TARGETDEPS += C:/local2/emiplib/lib/libemiplib.a



win32: LIBS += -LC:/local2/jthread/lib/ -llibjthread.dll

INCLUDEPATH += C:/local2/jthread/include
DEPENDPATH += C:/local2/jthread/include


win32: LIBS += -lWinMM

win32: LIBS += -LC:/local2/jrtplib/lib/ -llibjrtp.dll

INCLUDEPATH += C:/local2/jrtplib/include
DEPENDPATH += C:/local2/jrtplib/include
