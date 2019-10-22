QT += quick gui
CONFIG += c++11

DEFINES += USE_TCP
#else use udp

#choose one of them USE_DXGI / USE_GDI / USE_D3D
DEFINES += USE_DXGI
#DEFINES += USE_GDI
#DEFINES += USE_D3D

INCLUDEPATH += ../RemoteControl/src

HEADERS += \
    src/framelesswindow.h \
    src/networkapi.h \
    src/remoteevent.h \
    src/imageprovider.h \
    src/systemapi.h

SOURCES += \
    src/framelesswindow.cpp \
    src/main.cpp \
    src/networkapi.cpp \
    src/imageprovider.cpp \
    src/systemapi.cpp

if (contains(DEFINES, USE_TCP)){
    message("Use Tcp")
    HEADERS += \
        src/tcp/controlled.h \
        src/tcp/controller.h \
        src/tcp/protocol.h \
        src/tcp/socket.h

    SOURCES += \
        src/tcp/controlled.cpp \
        src/tcp/controller.cpp \
        src/tcp/protocol.cpp \
        src/tcp/socket.cpp

    INCLUDEPATH += src/tcp
}else{
    message("Use Udp")
    HEADERS += \
        src/udp/connection.h \
        src/udp/controlled.h \
        src/udp/controller.h \
        src/udp/protocol.h \
        src/udp/socket.h

    SOURCES += \
        src/udp/connection.cpp \
        src/udp/controlled.cpp \
        src/udp/controller.cpp \
        src/udp/protocol.cpp \
        src/udp/socket.cpp

    INCLUDEPATH += src/udp
}

win32{
    message("Platform: Win32")
    QT += winextras
    LIBS += -lGdi32
    RC_ICONS += image/winIcon.ico
    contains(DEFINES, USE_D3D){
        message("Duplication interface: D3D")
        HEADERS += src/dxgimanager.h
        LIBS += -ld3d9 -lD3dx9
    }
    contains(DEFINES, USE_DXGI){
        message("Duplication interface: DXGI")
        SOURCES += src/dxgimanager.cpp
        LIBS += -lD3D11 -lDXGI
    }
}else{
    message ("Platform: Android")
}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RESOURCES += \
    qml.qrc \
    image.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
