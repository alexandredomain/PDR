TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    xlstool.c \
    xls.c \
    ole.c \
    sqlite3.c \
    main.c \
    fonctions.c

HEADERS += \
    xlstypes.h \
    xlstool.h \
    xlsstruct.h \
    xls.h \
    ole.h \
    config.h \
    brdb.h \
    brdb.c.h \
    sqlite3.h \
    fonctions.h

LIBS += -ldl -lpthread

DESTDIR = $$PWD
