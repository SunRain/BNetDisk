TEMPLATE = app

QT += core qml quick network
CONFIG += c++11

DEFINES += QPM_INIT
OPTIONS += roboto

include(thirdparty/qsyncable/qsyncable.pri)
include(thirdparty/quickflux/quickflux.pri)
#include(thirdparty/qml-material/material.pri)
include(thirdparty/YADownloader/YADownloader.pri)

SOURCES += \
    src/main.cpp \
    src/BDiskRequest/BDiskLogin.cpp \
    src/BDiskRequest/BDiskTokenProvider.cpp \
    src/BDiskRequest/BDiskCookieJar.cpp \
    src/BDiskRequest/BDiskOperationRequest.cpp \
    src/BDiskRequest/BDiskHttpRequest.cpp \
    src/BDiskRequest/BDiskFileObject.cpp \
    src/BDiskRequest/BDiskFileDirOperation.cpp \
    src/BDiskRequest/BDiskBaseRequest.cpp \
    src/BDiskRequest/BDiskActions.cpp \
    src/BDiskDirListDelegate.cpp \
    src/ApplicationUtility.cpp \
    src/BDiskDownloadDelegate.cpp \
    src/BDiskEvent.cpp \
    src/BDiskShareDelegate.cpp \
    src/BDiskFileOperationDelegate.cpp \
    src/BDiskSearchDelegate.cpp

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
    src/BDiskRequest/BDiskCookieJar.h \
    src/BDiskRequest/BDiskOperationRequest.h \
    src/BDiskRequest/BDiskHttpRequest.h \
    src/BDiskRequest/BDiskFileObject.h \
    src/BDiskRequest/BDiskFileObjectKeys.h \
    src/BDiskRequest/BDiskFileDirOperation.h \
    src/BDiskRequest/BDiskBaseRequest.h \
    src/BDiskRequest/BDiskActions.h \
    src/BDiskDirListDelegate.h \
    src/ApplicationUtility.h \
    src/BDiskDownloadDelegate.h \
    src/BDiskEvent.h \
    src/BDiskShareDelegate.h \
    src/BDiskFileOperationDelegate.h \
    src/BDiskSearchDelegate.h

DISTFILES += \
    qml/QuickFlux/Stores/qmldir
