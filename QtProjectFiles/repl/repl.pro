TEMPLATE = app
CONFIG += console
CONFIG -= qt

LIBS += -lreadline -ltcmalloc

SOURCES += \
    ../../repl/main.cpp

HEADERS += \
    ../../include/Value.h \
    ../../include/Types.h \
    ../../include/Type.h \
    ../../include/Stdlib.h \
    ../../include/Stack.h \
    ../../include/Runnable.h \
    ../../include/PebbleScript.h \
    ../../include/Parser.h \
    ../../include/Operation.h \
    ../../include/NumericUtils.h \
    ../../include/Fallible.h \
    ../../include/Environment.h

