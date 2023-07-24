QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addplan.cpp \
    addteam.cpp \
    chat.cpp \
    displayplan.cpp \
    jointeam.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    search.cpp \
    send.cpp \
    signup.cpp \
    team.cpp \
    work.cpp



HEADERS += \
    addplan.h \
    addteam.h \
    chat.h \
    data.h \
    displayplan.h \
    jointeam.h \
    login.h \
    mainwindow.h \
    search.h \
    send.h \
    signup.h \
    team.h \
    work.h


FORMS += \
    addplan.ui \
    addteam.ui \
    chat.ui \
    displayplan.ui \
    jointeam.ui \
    login.ui \
    mainwindow.ui \
    search.ui \
    signup.ui \
    team.ui \
    work.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

