TEMPLATE = app

QT += qml quick quickcontrols2 sql concurrent network

android: QT += androidextras

CONFIG += c++11 qtquickcompiler

SOURCES += main.cpp \
    tasty.cpp \
    settings.cpp \
    apirequest.cpp \
    feedmodel.cpp \
    commentsmodel.cpp \
    datastructures.cpp \
    attachedimagesmodel.cpp \
    usersmodel.cpp \
    notificationsmodel.cpp \
    bayes.cpp \
    trainer.cpp \
    calendarmodel.cpp \
    cache/cachemanager.cpp \
    cache/cachedimage.cpp \
    stemmerv.cpp \
    usersmodeltlog.cpp \
    usersmodelbayes.cpp \
    androidnotifier.cpp

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
    commentsmodel.h \
    datastructures.h \
    attachedimagesmodel.h \
    usersmodel.h \
    notificationsmodel.h \
    bayes.h \
    trainer.h \
    calendarmodel.h \
    cache/cachemanager.h \
    cache/cachedimage.h \
    defines.h \
    stemmerv.h \
    usersmodeltlog.h \
    usersmodelbayes.h \
    androidnotifier.h

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/src/org/binque/taaasty/NotificationClient.java

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        $$PWD/../../android/libcrypto.so \
        $$PWD/../../android/libssl.so
}
