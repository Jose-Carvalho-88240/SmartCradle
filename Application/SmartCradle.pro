QT       += core gui
QT       += network
QT       += androidextras
QT       += avwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    databasehandler.cpp \
    main.cpp \
    mainwindow.cpp \
    notificationclient.cpp

HEADERS += \
    QNetworkAcessManagerWithPatch.h \
    databasehandler.h \
    mainwindow.h \
    notificationclient.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
android: include(C:/android/android_openssl-master/openssl.pri)


ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-sources

OTHER_FILES += \
    android-sources/src/org/qtproject/example/notification/NotificationClient.java \
    #android-sources/AndroidManifest.xml

DISTFILES += \
    android-sources/AndroidManifest.xml \
    android-sources/build.gradle \
    android-sources/gradle/wrapper/gradle-wrapper.jar \
    android-sources/gradle/wrapper/gradle-wrapper.properties \
    android-sources/gradlew \
    android-sources/gradlew.bat \
    android-sources/res/values/libs.xml

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/android-sources/libs/release/ -lQtAV
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/android-sources/libs/debug/ -lQtAV
else:unix: LIBS += -L$$PWD/android-sources/libs/ -lQtAV #-lQtAVWidgets

INCLUDEPATH += $$PWD/android-sources/libs
DEPENDPATH += $$PWD/android-sources/libs

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/android-sources/libs/release/ -lQtAVWidgets
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/android-sources/libs/debug/ -lQtAVWidgets
else:unix: LIBS += -L$$PWD/android-sources/libs/ -lQtAVWidgets

INCLUDEPATH += $$PWD/android-sources/libs
DEPENDPATH += $$PWD/android-sources/libs
