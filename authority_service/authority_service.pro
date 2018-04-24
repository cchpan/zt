TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $(TOOLKITS)/include
LIBS += -L$(TOOLKITS)/lib -lACE
LIBS += -lpthread

SOURCES += main.cpp \
    authority_handler.cpp \
    db_interface.cpp

HEADERS += \
    net_server.h \
    authority_handler.h \
    db_interface.h \
    net_svc_handler.h \
    creactor_task.h

DISTFILES += \
    CMakeLists.txt
