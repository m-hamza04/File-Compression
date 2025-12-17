QT       += core gui
QT += charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
TEMPLATE = app

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    abouthelp.cpp \
    compress.cpp \
    dashboard.cpp \
    decompression.cpp \
    history.cpp \
    huffman.cpp \
    imagecom.cpp \
    main.cpp \
    mainscreen.cpp \
    mainwindow.cpp \
    selectionscreen.cpp \
    theme.cpp \
    visualizer.cpp

HEADERS += \
    abouthelp.h \
    compress.h \
    dashboard.h \
    decompression.h \
    history.h \
    huffman.h \
    imagecom.h \
    mainscreen.h \
    mainwindow.h \
    selectionscreen.h \
    styledmessagebox.h \
    theme.h \
    visualizer.h


INCLUDEPATH += C:/opencv/build_mingw/include
LIBS += -LC:/opencv/build_mingw/x64/mingw/lib \
    -lopencv_core455 \
    -lopencv_imgcodecs455 \
    -lopencv_highgui455 \
    -lopencv_imgproc455\
    -lopencv_videoio455 \
    -lopencv_video455

