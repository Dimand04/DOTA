QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    1.cpp \
    deleteuser.cpp \
    global.cpp \
    heroitem.cpp \
    main.cpp \
    mainmenu.cpp \
    playershortinfo.cpp \
    qpushlabel.cpp

HEADERS += \
    deleteuser.h \
    global.h \
    heroitem.h \
    mainmenu.h \
    playershortinfo.h \
    qpushlabel.h

FORMS += \
    deleteuser.ui \
    heroitem.ui \
    mainmenu.ui \
    playershortinfo.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rsc.qrc

