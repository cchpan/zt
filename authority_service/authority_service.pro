TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $(TOOLKITS)/include
LIBS += -L$(TOOLKITS)/lib -lACE
LIBS += -lpthread -lmysqlclient

SOURCES += main.cpp \
    authority_handler.cpp \
    db_mysql_operation.cpp \
    authority_db_interface.cpp

HEADERS += \
    net_server.h \
    authority_handler.h \
    net_svc_handler.h \
    creactor_task.h \
    authority_handler_impl.h \
    db_mysql_operation.h \
    authority_db_interface.h

DISTFILES += \
    CMakeLists.txt
