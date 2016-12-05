#-------------------------------------------------
#
# Project created by QtCreator 2016-11-03T14:18:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MoveAPP
TEMPLATE = app


SOURCES += main.cpp\
        move.cpp

HEADERS  += move.h

FORMS    += move.ui

RC_FILE += move.rc

INCLUDEPATH += D:\\CPP_LIBS\\msvc10_vs2010\\lib\\opencv\\3_1_00\\include
LIBS += -LD:\\CPP_LIBS\\msvc10_vs2010\\lib\\opencv\\3_1_00\\x86\\Debug \
    -lopencv_bgsegm310d \
    -lopencv_bioinspired310d \
    -lopencv_calib3d310d \
    -lopencv_core310d \
    -lopencv_datasets310d \
    -lopencv_dpm310d \
    -lopencv_face310d \
    -lopencv_features2d310d \
    -lopencv_flann310d \
    -lopencv_fuzzy310d \
    -lopencv_highgui310d \
    -lopencv_imgcodecs310d \
    -lopencv_imgproc310d \
    -lopencv_line_descriptor310d \
    -lopencv_ml310d \
    -lopencv_objdetect310d \
    -lopencv_photo310d \
    -lopencv_plot310d \
    -lopencv_reg310d \
    -lopencv_rgbd310d \
    -lopencv_saliency310d \
    -lopencv_shape310d \
    -lopencv_stereo310d \
    -lopencv_stitching310d \
    -lopencv_superres310d \
    -lopencv_tracking310d \
    -lopencv_ts310d \
    -lopencv_video310d \
    -lopencv_videoio310d \
    -lopencv_videostab310d \
    -lopencv_xobjdetect310d

RESOURCES += \
    resources.qrc


win32: LIBS += -L$$PWD/tracker/Debug/ -lTracker

INCLUDEPATH += $$PWD/tracker
DEPENDPATH += $$PWD/tracker
