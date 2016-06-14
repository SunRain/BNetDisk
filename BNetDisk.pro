TEMPLATE = app

QT += core qml quick network
CONFIG += c++11

SOURCES += \
    src/main.cpp \
    src/BDiskRequest/BDiskLogin.cpp \
    src/BDiskRequest/BDiskTokenProvider.cpp \
    src/BDiskRequest/BDiskCookieJar.cpp

RESOURCES += \
    qml/qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    src/BDiskRequest/SingletonPointer.h \
    src/BDiskRequest/SingletonPointer_p.h \
    src/BDiskRequest/BDiskConst.h \
    src/BDiskRequest/BDiskLogin.h \
    src/BDiskRequest/BDiskTokenProvider.h \
    src/BDiskRequest/BDiskCookieJar.h
