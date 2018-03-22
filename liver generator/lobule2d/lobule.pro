QT += core gui xml widgets

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = lobule
TEMPLATE = app

LIBS += -L/usr/lib/x86_64-linux-gnu \

INCLUDEPATH += \
    base

SOURCES += main.cpp \
    mainwindow.cpp \
    base/baseform.cpp \
    base/geoeditor.cpp \
    base/project.cpp \
    base/settings.cpp \
    dialogclone.cpp \
    dialogproject.cpp \
    dialogsetting.cpp \
    formgeo.cpp \
    formproject.cpp

HEADERS  += mainwindow.h \
    base/baseform.h \
    base/geoeditor.h \
    base/project.h \
    base/settings.h \
    dialogclone.h \
    dialogproject.h \
    dialogsetting.h \
    formgeo.h \
    formproject.h

FORMS    += mainwindow.ui \
    formgeo.ui \
    formproject.ui \
    dialogclone.ui \
    dialogproject.ui \
    dialogsetting.ui

OTHER_FILES +=

RESOURCES += \
    resource.qrc

#copydata.commands = $(COPY_DIR) $$PWD/bin $$PWD/examples $$OUT_PWD
#first.depends = $(first) copydata
#export(first.depends)
#export(copydata.commands)
#QMAKE_EXTRA_TARGETS += first copydata
