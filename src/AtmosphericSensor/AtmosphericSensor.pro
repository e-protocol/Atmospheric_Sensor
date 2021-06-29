QT       += core gui\
            bluetooth\
            androidextras\
            multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bluetooth.cpp \
    main.cpp \
    mainwindow.cpp \
    sounds.cpp

HEADERS += \
    bluetooth.h \
    declaration.h \
    javarequest.h \
    mainwindow.h \
    sounds.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
android-sources/AndroidManifest.xml \
android-sources/ic_launcher.png

deployment.files += atmospherics_on.wav \
                    communications_on.wav \
                    fuzz.wav \
                    safe_day.wav

deployment.path = $$PWD/android-sources/assets
INSTALLS += deployment

ANDROID_PACKAGE_SOURCE_DIR=$$PWD/android-sources

RESOURCES +=
    android-sources/res/drawable-hdpi/splash.png

ANDROID_ABIS = armeabi-v7a
