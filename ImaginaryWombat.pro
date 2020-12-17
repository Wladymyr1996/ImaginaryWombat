QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    $$PWD/src/

SOURCES += \
    main.cpp \
    src/procesor/3dmodel.cpp \
    src/procesor/globalfunctions.cpp \
    src/procesor/imagehandler.cpp \
    src/procesor/langhandler.cpp \
    src/procesor/settingshandler.cpp \
    src/mainwindow.cpp \
    src/procesor/stlhandler.cpp \
    src/view/histogramview.cpp \
    src/view/imageview.cpp \
    src/view/tools.cpp \
    src/window/settingsdialog.cpp

HEADERS += \
    src/procesor/3dmodel.h \
    src/procesor/globalfunctions.h \
    src/procesor/imagehandler.h \
    src/procesor/langhandler.h \
    src/procesor/settingshandler.h \
    src/mainwindow.h \
    src/procesor/stlhandler.h \
    src/view/histogramview.h \
    src/view/imageview.h \
    src/view/tools.h \
    src/window/settingsdialog.h

TRANSLATIONS += \
    lang/en.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    languages.qrc
