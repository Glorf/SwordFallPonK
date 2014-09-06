TEMPLATE = app
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    mapa.cpp \
    main.cpp \
    gui.cpp \
    guiEvents.cpp \
    config.cpp \
    guiWidgets.cpp \
    base.cpp \
    engine.cpp

HEADERS += \
    mapa.h \
    gui.h \
    config.h \
    base.h \
    engine.h


QMAKE_CXXFLAGS += -std=c++11
INCLUDEPATH += /usr/include/allegro5
LIBS += -lallegro -lallegro_image -lallegro_primitives -lallegro_ttf -lallegro_font
