TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
#CONFIG -= qt

SOURCES += \
        eth_frame.cpp \
        ip_frame.cpp \
        main.cpp \
        monitor.cpp \
        scaner.cpp \
        tcp_frame.cpp \
        udp_frame.cpp

HEADERS += \
    eth_frame.h \
    ip_frame.h \
    monitor.h \
    scaner.h \
    tcp_frame.h \
    udp_frame.h

#LIBS += -lncurses
