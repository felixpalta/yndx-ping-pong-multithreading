TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -pthread  -fsanitize=thread -O2
LIBS += -lpthread
QMAKE_LFLAGS +=  -fsanitize=thread

SOURCES += \
        main.cpp
