TEMPLATE = app

QT += qml quick quickcontrols2 sql concurrent network websockets

android: QT += androidextras

!android: QT += widgets

CONFIG += c++11 qtquickcompiler

SOURCES += src/main.cpp \
    src/tasty.cpp \
    src/settings.cpp \
    src/apirequest.cpp \
    src/models/feedmodel.cpp \
    src/models/commentsmodel.cpp \
    src/models/attachedimagesmodel.cpp \
    src/models/usersmodel.cpp \
    src/models/notificationsmodel.cpp \
    src/nbc/bayes.cpp \
    src/nbc/trainer.cpp \
    src/models/calendarmodel.cpp \
    src/cache/cachemanager.cpp \
    src/cache/cachedimage.cpp \
    src/cache/cachedimageprovider.cpp \
    src/nbc/stemmerv.cpp \
    src/data/AttachedImage.cpp \
    src/data/Author.cpp \
    src/data/CalendarEntry.cpp \
    src/data/Comment.cpp \
    src/data/Entry.cpp \
    src/data/Media.cpp \
    src/data/Notification.cpp \
    src/data/Rating.cpp \
    src/data/Tlog.cpp \
    src/data/User.cpp \
    src/qpusher/channel.cpp \
    src/qpusher/pusher.cpp \
    src/data/Conversation.cpp \
    src/data/Message.cpp \
    src/models/chatsmodel.cpp \
    src/models/messagesmodel.cpp \
    src/pusherclient.cpp \
    src/models/flowsmodel.cpp \
    src/data/Flow.cpp \
    src/data/MessageBase.cpp \
    src/data/TastyData.cpp \
    src/models/tagsmodel.cpp \
    src/models/tastylistmodel.cpp \
    src/texthandler.cpp \
    src/poster.cpp \
    src/reposter.cpp \
    src/tastydatacache.cpp \
    src/models/uploadmodel.cpp \
    src/models/availabletlogsmodel.cpp

android: SOURCES += src/android/androidimagepicker.cpp \
    src/android/androidnotifier.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    src/tasty.h \
    src/settings.h \
    src/apirequest.h \
    src/models/feedmodel.h \
    src/models/commentsmodel.h \
    src/models/attachedimagesmodel.h \
    src/models/usersmodel.h \
    src/models/notificationsmodel.h \
    src/nbc/bayes.h \
    src/nbc/trainer.h \
    src/models/calendarmodel.h \
    src/cache/cachemanager.h \
    src/cache/cachedimage.h \
    src/cache/cachedimageprovider.h \
    src/defines.h \
    src/nbc/stemmerv.h \
    src/data/AttachedImage.h \
    src/data/Author.h \
    src/data/CalendarEntry.h \
    src/data/Comment.h \
    src/data/Entry.h \
    src/data/Media.h \
    src/data/Notification.h \
    src/data/Rating.h \
    src/data/Tlog.h \
    src/data/User.h \
    src/qpusher/channel.h \
    src/qpusher/pusher.h \
    src/data/Conversation.h \
    src/data/Message.h \
    src/models/chatsmodel.h \
    src/models/messagesmodel.h \
    src/pusherclient.h \
    src/models/flowsmodel.h \
    src/data/Flow.h \
    src/data/MessageBase.h \
    src/data/TastyData.h \
    src/models/tagsmodel.h \
    src/models/tastylistmodel.h \
    src/texthandler.h \
    src/poster.h \
    src/reposter.h \
    src/tastydatacache.h \
    src/models/uploadmodel.h \
    src/models/availabletlogsmodel.h \
    src/cache/lrucache.hpp

android: HEADERS += src/android/androidimagepicker.h \
    src/android/androidnotifier.h

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/src/org/binque/taaasty/NotificationClient.java \
    android/res/values/apptheme.xml \
    android/res/drawable/splash.xml \
    android/src/org/binque/taaasty/RealPathUtil.java

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        $$PWD/../../android/libcrypto.so \
        $$PWD/../../android/libssl.so
}

#unix:{
#    INCLUDEPATH += /home/binque/.Qt5/5.8/gcc_64/include/
#    INCLUDEPATH += /home/binque/.Qt5/5.8/gcc_64/include/QtConcurrent/
#    INCLUDEPATH += /home/binque/.Qt5/5.8/gcc_64/include/QtCore/
#    INCLUDEPATH += /home/binque/.Qt5/5.8/gcc_64/include/QtGui/
#    INCLUDEPATH += /home/binque/.Qt5/5.8/gcc_64/include/QtNetwork/
#    INCLUDEPATH += /home/binque/.Qt5/5.8/gcc_64/include/QtQml/
#    INCLUDEPATH += /home/binque/.Qt5/5.8/gcc_64/include/QtQuick/
#    INCLUDEPATH += /home/binque/.Qt5/5.8/gcc_64/include/QtQuickControls2/
#    INCLUDEPATH += /home/binque/.Qt5/5.8/gcc_64/include/QtSql/
#    INCLUDEPATH += /home/binque/.Qt5/5.8/gcc_64/include/QtWebSockets/
#    INCLUDEPATH += /home/binque/.Qt5/5.8/gcc_64/include/QtWidgets/

#    pvs_studio.target = Taaasty
#    pvs_studio.output = true
#    pvs_studio.sources = $${SOURCES}
#    pvs_studio.cxxflags = -fPIC
#    pvs_studio.cfg = "$$PWD/PVS-Studio.cfg"
#    include(PVS-Studio.pri)
#}
