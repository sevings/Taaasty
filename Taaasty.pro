TEMPLATE = app

QT += qml quick

CONFIG += c++11

SOURCES += main.cpp \
    tasty.cpp \
    settings.cpp \
    apirequest.cpp \
    feedmodel.cpp \
    imagecache.cpp \
    cachemanager.cpp \
    commentsmodel.cpp \
    datastructures.cpp \
    attachedimagesmodel.cpp \
    usersmodel.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    tasty.h \
    settings.h \
    apirequest.h \
    feedmodel.h \
    imagecache.h \
    cachemanager.h \
    commentsmodel.h \
    datastructures.h \
    attachedimagesmodel.h \
    usersmodel.h
