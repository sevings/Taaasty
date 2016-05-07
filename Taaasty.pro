TEMPLATE = app

QT += qml quick sql
android: QT += androidextras
CONFIG += c++11

CONFIG += qtquickcompiler

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
    usersmodel.cpp \
    notificationsmodel.cpp \
    bayes.cpp \
    trainer.cpp \
    calendarmodel.cpp

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
    usersmodel.h \
    notificationsmodel.h \
    bayes.h \
    trainer.h \
    calendarmodel.h

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
