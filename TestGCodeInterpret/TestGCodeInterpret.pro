QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../gcodelib/release/ -lgcodelib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../gcodelib/debug/ -lgcodelib
else:unix: LIBS += -L$$OUT_PWD/../gcodelib/ -lgcodelib
INCLUDEPATH += $$PWD/../gcodelib
DEPENDPATH += $$PWD/../gcodelib


macx: LIBS += -L$$PWD/../build-gcodelib-Desktop_Qt_5_15_1_clang_64bit-Debug/ -lgcodelib.1.0.0

INCLUDEPATH += $$PWD/../gcodelib
DEPENDPATH += $$PWD/../gcodelib
