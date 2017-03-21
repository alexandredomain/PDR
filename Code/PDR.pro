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
    traitementXLS.c \
    requetesSQL.c

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
    traitementXLS.h \
    requetesSQL.h

LIBS += -ldl -lpthread

DESTDIR = $$PWD
